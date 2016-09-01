#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "general.hpp"
#include "http_proto.hpp"
#include "http_parser.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace airobot {


enum connection_stats {
    conn_working = 1,
    conn_pending,
    conn_error,
};

class http_server;
class backend_server;
extern void show_front_conns_info(bool verbose);
extern void show_backend_conns_info(bool verbose);

class connection
{

public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// Construct a connection with the given socket.
    explicit connection(boost::shared_ptr<ip::tcp::socket> p_sock);

    virtual void start();
    virtual void stop() = 0;

    enum connection_stats get_stats() { return stats_; }
    void set_stats(enum connection_stats stat) { stats_ = stat; }

    void fill_and_send(const char* data, size_t len);
    void fill_for_http(const char* data, size_t len, const string& status);
    void fill_for_http(const string& str, const string& status);

    // some general tiny function
    bool set_tcp_nodelay(bool set_true);
    bool set_tcp_keepalive(bool set_true);

protected:
    // 异步IO
    virtual void do_read();
    virtual void do_write();

    virtual void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred) = 0;
    virtual void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) = 0;

    boost::shared_ptr<ip::tcp::socket> p_sock_;

    // 读写的有效负载记录
    size_t r_size_;
    size_t w_size_; size_t w_pos_; //写可能一次不能成功，这里保存已写的位置
    boost::shared_ptr<std::vector<char> > p_buffer_;
    boost::shared_ptr<std::vector<char> > p_write_;

    virtual ~connection();

    enum connection_stats stats_;

public:
    ptime touch_time_;

};

}


#endif //_CONNECTION_H_
