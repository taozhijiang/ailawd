#include "front_conn.hpp"
#include "reply.hpp"
#include "http_server.hpp"
#include "http_proto.hpp"
#include "json11.hpp"

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
    strand_(server.io_service_),
    connection(p_sock),
    parser_(),
    server_(server)
{
    p_buffer_ = boost::make_shared<std::vector<char> >(16*1024, 0);
    p_write_  = boost::make_shared<std::vector<char> >(16*1024, 0);
}

void front_conn::stop()
{
    //server_.set_session_id(shared_from_this(), (int64_t)-1);
    set_stats(conn_pending);
}


// 改写基类的读写
void front_conn::do_read()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    BOOST_LOG_T(info) << "strand read... in " << boost::this_thread::get_id();
    p_sock_->async_read_some(buffer(p_buffer_->data() + r_size_, p_buffer_->size() - r_size_),
                             strand_.wrap(
                                boost::bind(&front_conn::read_handler,
                                  this,
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

    BOOST_LOG_T(info) << "strand write... in " << boost::this_thread::get_id();
    p_sock_->async_write_some(buffer(*p_write_, w_size_),
                              strand_.wrap(
                                boost::bind(&front_conn::write_handler,
                                  this,
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred)));
    return;
}


void front_conn::read_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred)
    {
        if (p_sock_->available())
        {
            BOOST_LOG_T(info) << "socket still available: " << p_sock_->available(); 
            do_read();
            return;
        }

        r_size_ = 0;
        if (parser_.parse_request(p_buffer_->data()))
        {
            if (! boost::iequals(parser_.request_option(http_opts::request_method), "POST") )
            {
                BOOST_LOG_T(error) << "Invalid request method: " << parser_.request_option(http_opts::request_method);
                memcpy(p_write_->data(), reply::fixed_reply_bad_request.c_str(),
                       reply::fixed_reply_bad_request.size()+1);
                goto write_return;
            }

            if ( boost::iequals(parser_.request_option(http_opts::request_uri), "/ailaw") )
            {
                if( ailaw_handler() == false )
                {
                     BOOST_LOG_T(error) << "process error: " << parser_.request_option(http_opts::request_uri);
                     goto error_return;
                }

            }
            else if ( boost::iequals(parser_.request_option(http_opts::request_uri), "/analyse") )
            {
                if( analyse_handler() == false )
                {
                     BOOST_LOG_T(error) << "process error: " << parser_.request_option(http_opts::request_uri);
                     goto error_return;
                }
            }
            else if ( boost::iequals(parser_.request_option(http_opts::request_uri), "/item") )
            {
                if( item_handler() == false )
                {
                     BOOST_LOG_T(error) << "process error: " << parser_.request_option(http_opts::request_uri);
                     goto error_return;
                }
            }
            else
            {
                BOOST_LOG_T(error) << "Invalid request uri: " << parser_.request_option(http_opts::request_uri);
                memcpy(p_write_->data(), reply::fixed_reply_not_found.c_str(),
                       reply::fixed_reply_not_found.size()+1);
                goto write_return;
            }

            // default, OK
            goto write_return;
        }
        else
        {
            BOOST_LOG_T(error) << "Parse request error: " << p_buffer_->data() << endl;
            goto error_return;
        }
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "READ ERROR FOUND!";
        notify_conn_error();
        return;
    }

error_return:
    memcpy(p_write_->data(), reply::fixed_reply_error.c_str(),
           reply::fixed_reply_error.size()+1);

write_return:
    do_write();

    do_read();

    return;
}


void front_conn::write_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred)
    {
        assert(bytes_transferred == w_size_);
        w_size_ = 0;
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "WRITE ERROR FOUND!";
        notify_conn_error();
    }
}


void front_conn::notify_conn_error()
{
    {
        boost::lock_guard<boost::mutex> lock(server_.conn_notify_mutex);
        p_sock_->close();
        r_size_ = 0;
        w_size_ = 0;
        set_stats(conn_error);
    }
    server_.conn_notify.notify_one();
}


// 添加各种URI的handler
bool front_conn::ailaw_handler()
{
    string body = parser_.request_option(http_proto::header_options::request_body);
    string json_err;
    auto json_parsed = json11::Json::parse(body, json_err);

    // TO DO
    if (!json_err.empty())
    {
        BOOST_LOG_T(error) << "JSON parse error: " << body;
        return false;
    }

    if (!boost::equals(json_parsed["access_id"].string_value(), "11488058246"))
    {
        BOOST_LOG_T(error) << "Wrong access_id: " << json_parsed["access_id"].string_value();
        return false;
    }


    return false;
}

bool front_conn::analyse_handler()
{
    string body = parser_.request_option(http_proto::header_options::request_body);
    string json_err;
    auto json_parsed = json11::Json::parse(body, json_err);

    // TO DO
    if (!json_err.empty())
    {
        BOOST_LOG_T(error) << "JSON parse error: " << body;
        return false;
    }

    if (!boost::equals(json_parsed["access_id"].string_value(), "11488058246"))
    {
        BOOST_LOG_T(error) << "Wrong access_id: " << json_parsed["access_id"].string_value();
        return false;
    }

    // 和后端服务器同步连接和同步请求
    // 但是阻塞读取没有timed_out选项。。。
    BOOST_LOG_T(info) << "Request to host:" << win_server::ip << ":" << win_server::port;
    ip::tcp::socket sock(p_sock_->get_io_service());
    ip::tcp::endpoint ep(ip::address::from_string(win_server::ip),
                            win_server::port);
    boost::system::error_code error;

    sock.connect(ep, error);
    if (error)
    {
        BOOST_LOG_T(error) << "Connect to server failed!";
        return false;
    }

    //string msg = json_parsed["content"].string_value();
    //sock.write_some(buffer(msg));
    // 原样传递数据
    sock.write_some(buffer(body));

    boost::shared_ptr<std::vector<char> > p_buff = boost::make_shared<std::vector<char> >(16*1024, 0);
    size_t len = sock.read_some(buffer(*p_buff), error);
    if (error)
    {
        BOOST_LOG_T(error) << "READ from server failed!";
        return false;
    }

    fill_for_http(p_buff->data(), len);

    return true;
}

bool front_conn::item_handler()
{
    string body = parser_.request_option(http_proto::header_options::request_body);
    string json_err;
    auto json_parsed = json11::Json::parse(body, json_err);

    // TO DO
    if (!json_err.empty())
    {
        BOOST_LOG_T(error) << "JSON parse error: " << body;
        return false;
    }

    if (!boost::equals(json_parsed["access_id"].string_value(), "11488058246"))
    {
        BOOST_LOG_T(error) << "Wrong access_id: " << json_parsed["access_id"].string_value();
        return false;
    }


    return false;
}

}
