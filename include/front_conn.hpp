#ifndef _FRONT_CONN_H_
#define _FRONT_CONN_H_

#include "general.hpp"
#include "http_proto.hpp"
#include "http_parser.hpp"
#include "connection.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/enable_shared_from_this.hpp>

namespace airobot {

class http_server;
class front_conn;
typedef boost::shared_ptr<front_conn> front_conn_ptr;

class front_conn : public connection, public boost::enable_shared_from_this<front_conn>
{

public:
    front_conn(const front_conn&) = delete;
    front_conn& operator=(const front_conn&) = delete;

    /// Construct a connection with the given socket.
    front_conn(boost::shared_ptr<ip::tcp::socket> p_sock, http_server& server);

    void stop() override;
    virtual void read_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) override; 
    virtual void write_handler(const boost::system::error_code &ec, std::size_t bytes_transferred) override; 

    virtual ~front_conn() { BOOST_LOG_T(debug) << "FRONT SOCKET RELEASED!!!"; }

    void notify_conn_error();

private:
    http_parser parser_;
    http_server& server_;
};

}


#endif //_FRONT_CONN_H_
