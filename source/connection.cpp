#include "general.hpp"

#include "connection.hpp"
#include "reply.hpp"
#include "http_server.hpp"

#include "json11.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace airobot {

connection::connection(boost::shared_ptr<ip::tcp::socket> p_sock):
    touch_time_(second_clock::local_time()),
    p_sock_(p_sock),
    stats_(conn_pending)
{
    r_size_ = 0;
    w_size_ = 0;
    w_pos_  = 0;

    /**
     * 可以被后续resize增加
     */
    p_buffer_ = boost::make_shared<std::vector<char> >(16*1024, 0);
    p_write_  = boost::make_shared<std::vector<char> >(16*1024, 0);
}

void connection::start()
{
    /**
     * 这里需要注意的是，如果do_read()不是虚函数，而派生类只是简单的覆盖，
     * 那么在accept　handler中调用的new_c->start()会导致这里会调用基类
     * 版本的do_read
     */
    set_stats(conn_working);
    do_read();
}

void connection::do_read()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    p_sock_->async_read_some(buffer(*p_buffer_),
                             boost::bind(&connection::read_handler,
                                  this,
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred));
#if 0
    // 线程池多线程访问io_service模型
    p_sock_->async_read_some(buffer(*p_buffer_),
                             strand_.wrap(
                                boost::bind(&front_conn::read_handler,
                                  this,
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred)));
#endif
}

void connection::do_write()
{
    if (get_stats() != conn_working)
    {
        BOOST_LOG_T(error) << "SOCK STATUS: " << get_stats();
        return;
    }

    p_sock_->async_write_some(buffer(*p_write_),
                         boost::bind(&connection::write_handler,
                                  this,
                                  boost::asio::placeholders::error,
                                  boost::asio::placeholders::bytes_transferred));
}

void connection::fill_and_send(const char* data, size_t len)
{
    assert(data && len);
    if (len > p_write_->size())
        p_write_->resize(len);

    memcpy(p_write_->data(), data, len);
    w_size_ = len;
    w_pos_  = 0;

    do_write();
}


void connection::fill_for_http(const char* data, size_t len, const string& status = http_proto::status::ok)
{
    assert(data && len);

    string enc = reply::reply_generate(data, len, status);

    if (enc.size() + 1 > p_write_->size())
        p_write_->resize(enc.size() + 1);

    memcpy(p_write_->data(), enc.c_str(), enc.size()+1);    // copy '\0' but not transform

    w_size_ = enc.size();
    w_pos_  = 0;

    return;
}

void connection::fill_for_http(const string& str, const string& status = http_proto::status::ok)
{
    string enc = reply::reply_generate(str, status);

    if (enc.size() + 1 > p_write_->size())
        p_write_->resize(enc.size() + 1);

    memcpy(p_write_->data(), enc.c_str(), enc.size()+1); // copy '\0' but not transform

    w_size_ = enc.size();
    w_pos_  = 0;

    return;
}


connection::~connection()
{
    set_stats(conn_error);
}


}
