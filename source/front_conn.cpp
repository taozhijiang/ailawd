#include "front_conn.hpp"
#include "reply.hpp"
#include "http_server.hpp"
#include "http_proto.hpp"
#include "co_worker.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

namespace airobot {

namespace http_opts = http_proto::header_options;
namespace http_stat = http_proto::status;

front_conn::front_conn(boost::shared_ptr<ip::tcp::socket> p_sock,
                       http_server& server):
    connection(p_sock),
    request_(),
    parser_(),
    server_(server),
    strand_(boost::make_shared<io_service::strand>(server.io_service_))
{
    // p_buffer_ & p_write_
    // already allocated @ connection

    set_tcp_nodelay(true);
    //assert(set_tcp_keepalive(true));
}

void front_conn::start()
{
    /**
     * 这里需要注意的是，如果do_read()不是虚函数，而派生类只是简单的覆盖，
     * 那么在accept　handler中调用的new_c->start()会导致这里会调用基类
     * 版本的do_read
     */
    set_stats(conn_working);
    r_size_ = w_size_ = w_pos_ = 0;
    do_read_head();
}

void front_conn::stop()
{
    //server_.set_session_id(shared_from_this(), (int64_t)-1);
    set_stats(conn_pending);
}

// Wrapping the handler with strand.wrap. This will return a new handler, that will dispatch through the strand.
// Posting or dispatching directly through the strand.

void front_conn::do_read_head()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    touch_sock_time();
    BOOST_LOG_T(info) << "strand read read_util ... in " << boost::this_thread::get_id();
    async_read_until(*p_sock_, request_,
                             "\r\n\r\n",
                             strand_->wrap(
                                 boost::bind(&front_conn::read_head_handler,
                                     shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred)));
    return;
}


void front_conn::do_read_body()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    touch_sock_time();
    size_t len = ::atoi(parser_.request_option(http_opts::content_length).c_str());

    BOOST_LOG_T(info) << "strand read async_read exactly... in " << boost::this_thread::get_id();
    async_read(*p_sock_, buffer(p_buffer_->data() + r_size_, len - r_size_),
                    boost::asio::transfer_at_least(len - r_size_),
                             strand_->wrap(
                                 boost::bind(&front_conn::read_body_handler,
                                     shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred)));
    return;
}

void front_conn::do_write()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    touch_sock_time();
    assert(w_size_);
    assert(w_pos_ < w_size_);

    BOOST_LOG_T(info) << "strand write async_write exactly... in " << boost::this_thread::get_id();
    async_write(*p_sock_, buffer(p_write_->data() + w_pos_, w_size_ - w_pos_),
                    boost::asio::transfer_at_least(w_size_ - w_pos_),
                              strand_->wrap(
                                 boost::bind(&front_conn::write_handler,
                                     shared_from_this(),
                                     boost::asio::placeholders::error,
                                     boost::asio::placeholders::bytes_transferred)));
    return;
}

void front_conn::read_head_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred)
    {
        std::string head_str (boost::asio::buffers_begin(request_.data()),
                              boost::asio::buffers_begin(request_.data()) + request_.size());

        request_.consume(bytes_transferred); // skip the head

        if (parser_.parse_request(head_str.c_str()))
        {
            if (! boost::iequals(parser_.request_option(http_opts::request_method), "POST") )
            {
                BOOST_LOG_T(error) << "Invalid request method: " << parser_.request_option(http_opts::request_method);
                fill_for_http(http_proto::content_bad_request, http_proto::status::bad_request);
                goto write_return;
            }

            if ( boost::iequals(parser_.request_option(http_opts::request_uri), "/ailaw") ||
                 boost::iequals(parser_.request_option(http_opts::request_uri), "/analyse") ||
                 boost::iequals(parser_.request_option(http_opts::request_uri), "/item") )
            {
                size_t len = ::atoi(parser_.request_option(http_opts::content_length).c_str());
                r_size_ = 0;
                size_t additional_size = request_.size();
                assert( additional_size <= len );

                if (len + 1 > p_buffer_->size())
                {
                    BOOST_LOG_T(info) << "relarge receive buffer size to: " << (len + 256) << endl;
                    p_buffer_->resize(len + 256);
                }

                // first async_read_until may read more additional data, if so
                // then move additional data possible
                if( additional_size )
                {
                    std::string additional (boost::asio::buffers_begin(request_.data()),
                              boost::asio::buffers_begin(request_.data()) + additional_size);

                    memcpy(p_buffer_->data(), additional.c_str(), additional_size + 1);
                    request_.consume(additional_size); // skip the head
                }

                // normally, we will return these 2 cases
                if (additional_size < len)
                {
                    // need to read more data
                    r_size_ = additional_size;
                    do_read_body();
                    return;
                }
                else
                {
                    // call the process callback directly
                    read_body_handler(ec, additional_size);   // already updated r_size_
                    return;
                }
            }
            else
            {
                BOOST_LOG_T(error) << "Invalid request uri: " << parser_.request_option(http_opts::request_uri);
                fill_for_http(http_proto::content_not_found, http_proto::status::not_found);
                goto write_return;
            }

            // default, OK
            goto write_return;
        }
        else
        {
            BOOST_LOG_T(error) << "Parse request error: " << head_str << endl;
            goto error_return;
        }
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "READ ERROR FOUND!";

        boost::system::error_code ignored_ec;
        p_sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ignored_ec);
        p_sock_->cancel();

        notify_conn_error();
        return;
    }

error_return:
    fill_for_http(http_proto::content_error, http_proto::status::internal_server_error);
    request_.consume(request_.size());
    r_size_ = 0;

write_return:
    do_write();

    // if error, we will not read anymore
    // 此处关闭连接，消息会不会确保能写回到浏览器去???
    // notify_conn_error(); 算了，还是重新发起一个读，正常情况客户端应该会主动断开
    start();

    return;
}


void front_conn::read_body_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred)
    {

        size_t len = ::atoi(parser_.request_option(http_opts::content_length).c_str());
        r_size_ += bytes_transferred;

        if (r_size_ < len)
        {
            // need to read more!
            do_read_body();
            return;
        }

        if ( boost::iequals(parser_.request_option(http_opts::request_uri), "/analyse") )
        {
            if( analyse_handler() == false )
            {
                 BOOST_LOG_T(error) << "process error: " << parser_.request_option(http_opts::request_uri);
                 goto error_return;
            }
        }
        else
        {
            BOOST_LOG_T(error) << "Invalid request uri: " << parser_.request_option(http_opts::request_uri);
            fill_for_http(http_proto::content_not_found, http_proto::status::not_found);
            goto write_return;
        }

        // default, OK
        goto write_return;
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "READ ERROR FOUND: " << ec;

        boost::system::error_code ignored_ec;
        p_sock_->shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ignored_ec);
        p_sock_->cancel();

        notify_conn_error();
        return;
    }

error_return:
    fill_for_http(http_proto::content_error, http_proto::status::internal_server_error);

write_return:
    do_write();
    
    // 同上
    start();

    return;
}


void front_conn::write_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred)
    {
        //assert(bytes_transferred == w_size_);
        //w_size_ = 0;

        w_pos_ += bytes_transferred;

        if (w_pos_ < w_size_)
        {
            BOOST_LOG_T(error) << "ADDITIONAL WRITE: " << w_pos_ << " ~ " << w_size_;
            do_write();
        }
        else
        {
            w_pos_ = w_size_ = 0;
        }
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "WRITE ERROR FOUND:" << ec;

        boost::system::error_code ignored_ec;
        p_sock_->shutdown(ip::tcp::socket::shutdown_send, ignored_ec);
        p_sock_->cancel();

        notify_conn_error();
    }
}

void front_conn::notify_conn_error()
{
    r_size_ = 0;
    w_size_ = 0;
    set_stats(conn_error);

    server_.push_to_remove(shared_from_this());
    server_.conn_notify.notify_one();
}

void front_conn::conn_wash_white()
{
    assert( get_stats() != conn_working );

    r_size_ = w_size_ = w_pos_ = 0;
    p_buffer_->resize(16*1024);
    p_write_->resize(16*1024);

    //request_.

    stats_ = conn_pending;

    p_sock_.reset();
    strand_.reset();

    return;
}

void front_conn::conn_reset_network(boost::shared_ptr<ip::tcp::socket> p_sock)
{
    p_sock_ = p_sock;
    strand_ = boost::make_shared<io_service::strand>(server_.io_service_);

    // 暂时没有想到更好的重新初始化streambuf的方法，如果后面是在不行的话
    // 就用shared_ptr每次重新申请
    if (request_.size())
        request_.consume(request_.size());

    touch_sock_time();
    return;
}

// 添加各种URI的handler
bool front_conn::analyse_handler()
{
    string body = string(p_buffer_->data(), r_size_);
    body = string(body.c_str());

    if (body.find("access_id") == std::string::npos ||
        body.find("11488058246") == std::string::npos )
    {
        BOOST_LOG_T(error) << "Wrong access_id detected! " << body;
        return false;
    }

    // 和后端服务器同步连接和同步请求
    // 但是阻塞读取没有timed_out选项。。。
    BOOST_LOG_T(info) << "Request to host:" << win_server::ip << ":" << win_server::port;

    ip::tcp::socket sock(p_sock_->get_io_service());
    ip::tcp::endpoint ep(ip::address::from_string(win_server::ip),
                            win_server::port);
    boost::system::error_code error;

    // already blocked connect
    //sock.connect(ep, error);
    //if (error)
    //{
    //    BOOST_LOG_T(error) << "Connect to server failed!";
    //    return false;
    //}

    class co_worker *p_worker = server_.get_co_worker();
    sync_timed_connect(p_worker, ep , sock, 1000, error);

    if (error)
    {
        BOOST_LOG_T(error) << "Connect to server failed(1000ms)!";
        return false;
    }

    //string msg = json_parsed["content"].string_value();
    //sock.write_some(buffer(msg));
    // 原样传递数据
    sock.write_some(buffer(body));

    std::vector<char> read_buff(32*1024, 0);
    size_t read_len = sync_read_from(sock, read_buff, error);

    //size_t read_len = sync_timed_read_from(p_worker,
    //                                       sock, read_buff, 7000, error);

    if (read_len == 0)
    {
        BOOST_LOG_T(info) << "Read operation returned 0!";
        return false;
    }

    BOOST_LOG_T(info) << "Transform answer from server with size: " << read_len ;
    string ret_str(read_buff.data(), read_len);
    ret_str = string(ret_str.c_str());
    fill_for_http(ret_str, http_proto::status::ok);

    return true;
}


}
