#ifndef _FRONT_CONN_H_
#define _FRONT_CONN_H_

#include "general.hpp"
#include "http_proto.hpp"
#include "http_parser.hpp"
#include "connection.hpp"

#include "client_ops.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/enable_shared_from_this.hpp>

namespace airobot {

class http_server;
class front_conn;
typedef boost::shared_ptr<front_conn> front_conn_ptr;
typedef boost::weak_ptr<front_conn>   front_conn_weak;

class front_conn : public connection, public client_ops, 
                   public boost::enable_shared_from_this<front_conn>
{

public:
    front_conn(const front_conn&) = delete;
    front_conn& operator=(const front_conn&) = delete;

    /// Construct a connection with the given socket.
    front_conn(boost::shared_ptr<ip::tcp::socket> p_sock, http_server& server);

    virtual void start() override;
    virtual void stop() override;
    virtual void do_read() override  { /* Not Permit */ abort(); }
    virtual void do_write() override;

    void do_read_head();
    void do_read_body();

    void read_head_handler(const boost::system::error_code &ec, std::size_t bytes_transferred); 
    void read_body_handler(const boost::system::error_code &ec, std::size_t bytes_transferred); 
    virtual void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred) override 
    { /* Not Permit */ abort(); }
    virtual void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) override; 

    virtual ~front_conn() { BOOST_LOG_T(debug) << "FRONT SOCKET RELEASED!!!"; }

    void notify_conn_error();
    void conn_wash_white();
    void conn_reset_network(boost::shared_ptr<ip::tcp::socket> p_sock);

private:
    // uri handler，如果成功返回true，对应的回复消息封装在_write中，直接调用
    // do_write发送，否则内容未定义，手动覆盖生成错误回复信息
    bool ailaw_handler();
    bool analyse_handler();
    bool item_handler();

    // 用于读取HTTP的头部使用
    boost::asio::streambuf request_;   // client request_ read

private:
    http_parser parser_;
    http_server& server_;

    boost::shared_ptr<io_service::strand> strand_;
};

}


#endif //_FRONT_CONN_H_
