#ifndef _HTTP_SERVER_HPP
#define _HTTP_SERVER_HPP

#include "general.hpp"
#include <boost/bind.hpp>
#include <set>

#include "front_conn.hpp"
#include "co_worker.hpp"

//不用bimap了
#include <boost/unordered_map.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include <boost/circular_buffer.hpp>

namespace airobot {

using namespace boost::asio;

class http_server
{
public:
    //不准拷贝、赋值
    http_server(const http_server&) = delete;
    http_server& operator=(const http_server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit http_server(const objects* daemons, const std::string& address, unsigned short port,
                    const std::string& doc_root, size_t c_cz);

    /// Run the server's io_service loop.
    void run();

    int64_t request_session_id(front_conn_ptr ptr);
    bool set_session_id(front_conn_ptr ptr, uint64_t session_id);
    front_conn_ptr request_connection(uint64_t session_id);

    class co_worker* get_co_worker() { return daemons_->co_worker_; }

    void show_conns_info(bool verbose);

    static boost::condition_variable_any conn_notify;
    static boost::mutex conn_notify_mutex;
    // 等待和保护的条件是http_server::pending_to_remove_

private:
    friend class front_conn;

    io_service io_service_;

    // 侦听地址信息
    ip::tcp::endpoint ep_;
    ip::tcp::acceptor acceptor_;

    size_t concurr_sz_;

    void do_accept();
    void accept_handler(const boost::system::error_code& ec, socket_ptr ptr);

    friend void manage_thread(const objects* daemons);

    boost::mutex    front_conns_mutex_;
    boost::unordered_map<front_conn_ptr, size_t> front_conns_;

    // 记录front_conns_中的连接数目，便于控制最大服务量
    // 下面这些数据结构同样被front_conns_mutex_保护
    unsigned long long max_serve_conns_cnt_;
    unsigned long long current_conns_cnt_;


    // 每次需要删除的conn都放到这个set中，避免manage线程
    // 每次持锁遍历整个连接容器，提高效率。 使用set是为了防止多次插入
    std::set<front_conn_ptr> pending_to_remove_;
    void push_to_remove(front_conn_ptr ptr)
    {
        // 因为可能出错，超时都导致添加，这里不需要assert
        //assert( pending_to_remove_.find(ptr) == pending_to_remove_.end());
        boost::lock_guard<boost::mutex> lock(conn_notify_mutex);
        pending_to_remove_.insert(ptr);
    }

    size_t conn_expired_time_;  // in second
    // circular_buffer从尾部push_back的时候，会自动将头部的元素pop掉
    // 这里我们的思路是，把新连接插入到当前的bucket当中，然后随着时间的推迟，
    // 如果在下一轮进行检测的时候，该conn的touch_time大于过期时间，就从
    // front_conns_取出让其析构之。这里剔除的时间可能不够准确是规定时间，但是效率较高
    friend void co_worker::timing_wheel_check(const boost::system::error_code& ec);
    boost::circular_buffer<std::set<front_conn_weak>> timing_wheel_;

    //std::set<connection_ptr> connections_;
    //std::map<unsigned long long session_id, connection_ptr> connections_;

    // 缓存空闲的front_conn对象，最大大小为max_serve_conns_cnt_的30%
    std::vector<front_conn_ptr> cached_conns_;

    const objects* daemons_;

public:

    // 使用limit获得当前进程最大句柄的硬限制
    bool set_max_serve_conns_cnt(unsigned long long cnt);

    // must called before co_worker thread start
    void set_conn_expired_time(size_t new_time) {
        if ( new_time > timing_wheel_.capacity())
        {
            conn_expired_time_ = new_time;
        }
    }
    size_t get_conn_expired_time() { return conn_expired_time_; }

    size_t get_conn_check_spare()
    {
        assert(conn_expired_time_/timing_wheel_.capacity());
        return (conn_expired_time_/timing_wheel_.capacity());
    }

};

} // END NAMESPACE

#endif //_HTTP_SERVER_HPP
