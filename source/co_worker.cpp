#include "general.hpp"
#include "co_worker.hpp"

namespace airobot {

co_worker::co_worker():
    io_service_(),
    signal_(io_service_, /*SIGINT,*/ SIGTERM)
{
    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));

    return;
}

void co_worker::signal_handler(const boost::system::error_code& error, 
                               int sig_number)
{
    cerr << "SIGNAL:" << sig_number << "RECEIVED!" << endl;

    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));
}

}

