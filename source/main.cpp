#include "general.hpp"

#include "http_server.hpp"
#include "co_worker.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace airobot {

void manage_thread(const objects* daemons);
void boost_log_init(const string filename);

void backtrace_init();

}

// global variable
airobot::objects all_daemons = {0, 0};

int main(int argc, char* argv[])
{
    const string ip_addr = "0.0.0.0";
    unsigned short srv_port = 8911;
    const string doc_root = "./";

    // ignore sigpipe
    ::signal(SIGPIPE, SIG_IGN);
    airobot::boost_log_init("airobot_running");

    airobot::backtrace_init();

    boost::thread_group threads;

    size_t concurr_num = boost::thread::hardware_concurrency();
    BOOST_LOG_T(info) << "System hardware_concurrency: " << concurr_num;

    try
    {
        BOOST_LOG_T(info) << "Server Runing At:" << ip_addr << ":" << srv_port;
        BOOST_LOG_T(info) << "DocumentRoot:" << doc_root;

        all_daemons.http_server_ = new airobot::http_server(&all_daemons,
                                                            ip_addr, srv_port, doc_root, concurr_num);
        // 在这里先设置，确保能先于co_worker线程启动前生效
        all_daemons.http_server_->set_max_serve_conns_cnt(500);
        all_daemons.http_server_->set_conn_expired_time(600);

        all_daemons.co_worker_   = new airobot::co_worker(&all_daemons);

        threads.create_thread(boost::bind(&airobot::http_server::run, all_daemons.http_server_));
        threads.create_thread(boost::bind(&airobot::co_worker::run, all_daemons.co_worker_));
        threads.create_thread(boost::bind(airobot::manage_thread, &all_daemons));

        threads.join_all();
    }
    catch (std::exception& e)
    {
        BOOST_LOG_T(fatal) << "exception: " << e.what() << endl;;
    }


    return 0;
}
