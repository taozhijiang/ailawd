#include "general.hpp"

#include <http_server.hpp>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace airobot {

void manage_thread(boost::shared_ptr<http_server> p_srv);
void boost_log_init(const string filename);

}


int main(int argc, char* argv[])
{
    const string ip_addr = "0.0.0.0";
    unsigned short srv_port = 5598;
    const string doc_root = "./";

    airobot::boost_log_init("ailawd_running");

    boost::thread_group threads;

    size_t concurr_num = boost::thread::hardware_concurrency();
    BOOST_LOG_T(info) << "System hardware_concurrency: " << concurr_num;

    try
    {
        BOOST_LOG_T(info) << "Server Runing At:" << ip_addr << ":" << srv_port;
        BOOST_LOG_T(info) << "DocumentRoot:" << doc_root;
        //airobot::http_server srv(ip_addr, srv_port, doc_root);
        boost::shared_ptr<airobot::http_server> p_srv =
            boost::make_shared<airobot::http_server>(ip_addr, srv_port, doc_root, concurr_num);

        threads.create_thread(
            [&p_srv]{
            cerr<<boost::this_thread::get_id()<<endl;
            p_srv->run();
        });

        threads.create_thread(boost::bind(airobot::manage_thread, p_srv));

        threads.join_all();
    }
    catch (std::exception& e)
    {
        BOOST_LOG_T(fatal) << "exception: " << e.what() << endl;;
    }


    return 0;
}
