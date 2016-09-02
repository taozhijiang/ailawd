#include <iostream>

#include "http_server.hpp"
#include "front_conn.hpp"

#include <boost/format.hpp>
#include <boost/thread.hpp>

namespace airobot {

boost::condition_variable_any http_server::conn_notify;
boost::mutex http_server::conn_notify_mutex;

http_server::http_server(const objects* daemons, 
                         const std::string& address, unsigned short port,
                         const std::string& doc_root, size_t c_cz) :
    io_service_(),
    ep_(ip::tcp::endpoint(ip::address::from_string(address), port)),
    acceptor_(io_service_, ep_),
    concurr_sz_(c_cz*2),
    front_conns_mutex_(),
    front_conns_(),
    max_serve_conns_cnt_(1024),
    current_conns_cnt_(0),
    pending_to_remove_(),
    timing_wheel_(10),  //决定了检查的颗粒度
    daemons_(daemons)
{
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
    acceptor_.listen();

    // 数据库连接池初始化部分
    sql_conns_ =  boost::make_shared<aisqlpp::conns_manage>(c_cz+2, "127.0.0.1", "v5kf", "v5kf", "v5_law");
    string str = "SELECT count(uuid) FROM v5_law_text;";
    boost::shared_ptr<aisqlpp::connection> ptr = sql_conns_->request_conn(); 
    size_t cnt = ptr->execute_query_count(str);
    BOOST_LOG_T(info) << "TOTAL cases in database: " << cnt << endl;
    sql_conns_->free_conn(ptr);

    do_accept();
}

/// Run the server's io_service loop.

namespace http_stats = http_proto::status;

void http_server::run()
{
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread> > threads_pool;
    for (std::size_t i = 0; i < concurr_sz_; ++i) 
    {
        boost::shared_ptr<boost::thread> c_thread (
            new boost::thread(
                boost::bind(&boost::asio::io_service::run, &io_service_)));

        threads_pool.push_back(c_thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < concurr_sz_; ++i)
        threads_pool[i]->join();

    // single-threaded
    //io_service_.run();
}

void http_server::do_accept()
{
    socket_ptr p_sock(new ip::tcp::socket(io_service_));
    acceptor_.async_accept(*p_sock,
                           boost::bind(&http_server::accept_handler, this,
                                       boost::asio::placeholders::error, p_sock));
}

void http_server::accept_handler(const boost::system::error_code& ec, socket_ptr p_sock)
{
    if (ec)
    {
        BOOST_LOG_T(error) << "Error during accept!";
        return;
    }

    BOOST_LOG_T(debug) << "Client Info: " << p_sock->remote_endpoint().address() << "/" <<
        p_sock->remote_endpoint().port();

    if (current_conns_cnt_ >= max_serve_conns_cnt_) 
    {
        BOOST_LOG_T(error) << boost::format("We only support %llu, current %llu") % max_serve_conns_cnt_
            % current_conns_cnt_ << endl;
        p_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        p_sock->close();

        // 再次启动接收异步请求
        do_accept();
        return;
    }

    front_conn_ptr new_c = boost::make_shared<front_conn>(p_sock, *this);

    {
        std::lock_guard<std::mutex> lock(front_conns_mutex_);
        front_conns_.left.insert(std::make_pair(new_c, 0ULL));
        current_conns_cnt_ ++;
    }
    new_c->start();

    // 再次启动接收异步请求
    do_accept();
}


int64_t http_server::request_session_id(front_conn_ptr ptr)
{
    std::lock_guard<std::mutex> lock(front_conns_mutex_);

    auto p = front_conns_.left.find(ptr);
    if (p == front_conns_.left.end())
        return -1LL;

    return p->second;
}


bool http_server::set_session_id(front_conn_ptr ptr, uint64_t session_id)
{
    std::lock_guard<std::mutex> lock(front_conns_mutex_);

    auto p = front_conns_.left.find(ptr);
    if (p == front_conns_.left.end())
        return false;

    front_conns_.left.replace_data(p, session_id);
    return true;
}

front_conn_ptr http_server::request_connection(uint64_t session_id)
{
    assert(session_id != 0);
    assert(static_cast<int64_t>(session_id) != -1);

    std::lock_guard<std::mutex> lock(front_conns_mutex_);

    // ::right_iterator
    auto p = front_conns_.right.find(session_id);

    if (p == front_conns_.right.end())
        return nullptr;

    assert(front_conns_.right.count(session_id) == 1);
    //cout << typeid(p).name() << endl;

    return p->second;
}


void http_server::show_conns_info(bool verbose)
{
    size_t total_cnt = 0, err_cnt = 0, work_cnt = 0, pend_cnt = 0;
//    size_t normal_cnt = 0, zero_cnt = 0, negone_cnt = 0;
     
    std::lock_guard<std::mutex> lock(front_conns_mutex_);

    front_conn_type::left_map& view = front_conns_.left;

    for (auto const_iter = view.begin(); const_iter != view.end(); ++const_iter)
    {
        if (verbose) 
            cout << boost::format("front_conn[%d], touched:%s, status: ")
                                  % total_cnt % to_simple_string(const_iter->first->touch_time_); 
        
        if (const_iter->first->get_stats() == conn_working)
        { work_cnt++; if (verbose) cout << "working" ; }
        if (const_iter->first->get_stats() == conn_pending)
        { pend_cnt++; if (verbose) cout << "pending" ; }
        if (const_iter->first->get_stats() == conn_error)
        { err_cnt++;  if (verbose) cout << "error" ; }

//        cout << endl << "\t";
//        if ((int64_t)const_iter->second == 0)
//        { zero_cnt++;   if (verbose) cout << "session: 0" << endl; }
//        if ((int64_t)const_iter->second == -1)
//        { negone_cnt++; if (verbose) cout << "session: -1" << endl; }
//        else
//        { normal_cnt++; if (verbose) cout << "session: " << const_iter->second << endl; }

        total_cnt ++;
    }

    cout << boost::format("[FRONT_SERVER  ] total:%d, working:%d, pending:%d, error:%d ")
        % total_cnt % work_cnt % pend_cnt % err_cnt << endl;

    // not used
    //cout << boost::format("\tsession, normal:%d, 0:%d, -1:%d ")
    //    % normal_cnt % zero_cnt % negone_cnt << endl;

    return;
}

#include <sys/resource.h>
bool http_server::set_max_serve_conns_cnt(unsigned long long cnt)
{
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) != 0) 
    {
        BOOST_LOG_T(error) << "getrlimit() failed with errno:" << errno;
        return false;
    }

    if (cnt > limit.rlim_cur)  // 并没有考虑额外占用的句柄数目
    {
        BOOST_LOG_T(error) << "Reach the soft_limit of nofile:" << limit.rlim_cur; 
        return false;
    }

    max_serve_conns_cnt_ = cnt;
    BOOST_LOG_T(info) << "Current Server Maxium Serve Count: " << max_serve_conns_cnt_;
    BOOST_LOG_T(info) << "The limit is: " << limit.rlim_cur;

    return true;
}

} // END NAMESPACE


