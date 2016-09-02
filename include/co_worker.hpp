#ifndef _CO_WORKER_HPP
#define _CO_WORKER_HPP

#include "general.hpp"

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * 协作线程，主要使用asio异步处理定时器、信号等问题
 */

namespace airobot {

class co_worker: public boost::noncopyable
{
public:
    explicit co_worker(const objects* daemons);

    void run() { io_service_.run(); }

    void signal_handler(const boost::system::error_code& error, 
                        int signal_number);

    void timed_cancel_socket(const boost::system::error_code& ec,
                                    boost::asio::deadline_timer* t, ip::tcp::socket* sock);

    io_service& get_io_service() { return  io_service_; }


private:
    io_service io_service_;
    signal_set signal_;
    const objects* daemons_;
};

}


#endif
