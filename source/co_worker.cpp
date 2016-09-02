#include "general.hpp"
#include "co_worker.hpp"

namespace airobot {

co_worker::co_worker(const objects* daemons):
    io_service_(),
    signal_(io_service_, /*SIGINT,*/ SIGTERM),
    daemons_(daemons)
{
    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));

    return;
}

void co_worker::signal_handler(const boost::system::error_code& error, 
                               int sig_number)
{
    BOOST_LOG_T(info) << "SIGNAL:" << sig_number << "RECEIVED!" << endl;

    // re-listen again!
    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));
}

void co_worker::timed_cancel_socket(const boost::system::error_code& ec,
                                    boost::asio::deadline_timer* t, ip::tcp::socket* sock)
{
    if (ec == boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(info) << "timed_cancel_socket cancelled!";
        return;
    }

    BOOST_LOG_T(info) << "timed_cancel_socket callback called!";
    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);

    return;
}

}

