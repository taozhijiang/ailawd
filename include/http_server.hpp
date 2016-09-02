#ifndef _HTTP_SERVER_HPP
#define _HTTP_SERVER_HPP

#include "general.hpp"
#include <boost/bind.hpp>
#include <set>
#include <mutex>

#include "front_conn.hpp"

#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "aisql_conns_manage.hpp"
#include "aisql_connection.hpp"

namespace airobot {

using namespace boost::asio;

static constexpr int FRONT_EXPIRED_INTERVEL = 30*60; //30min

class http_server
{
public:
    //不准拷贝、赋值
    http_server(const http_server&) = delete;
    http_server& operator=(const http_server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit http_server(const objects* daemons, const std::string& address, unsigned short port,
                    const std::string& doc_root, size_t c_cz);

    /// Run the server's io_service loop.
    void run();

    int64_t request_session_id(front_conn_ptr ptr);
    bool set_session_id(front_conn_ptr ptr, uint64_t session_id);
    front_conn_ptr request_connection(uint64_t session_id);
    aisqlpp::conns_manage& get_sql_manager() { return *sql_conns_; }
    
    class co_worker* get_co_worker() { return daemons_->co_worker_; }

    void show_conns_info(bool verbose);

    static boost::condition_variable_any conn_notify;
    static boost::mutex conn_notify_mutex;

private:
    friend class front_conn;

    io_service io_service_;

    ip::tcp::endpoint ep_;
    ip::tcp::acceptor acceptor_;

    size_t concurr_sz_;

    void do_accept();
    void accept_handler(const boost::system::error_code& ec, socket_ptr ptr);

    typedef boost::bimap< boost::bimaps::set_of<front_conn_ptr>,
                          boost::bimaps::multiset_of<uint64_t> > front_conn_type;

    friend void manage_thread(const objects* daemons);

    front_conn_type front_conns_;
    std::mutex      front_conns_mutex_;
    //std::set<connection_ptr> connections_;
    //std::map<unsigned long long session_id, connection_ptr> connections_;

    boost::shared_ptr<aisqlpp::conns_manage> sql_conns_;

    const objects* daemons_;   
};

} // END NAMESPACE

#endif //_HTTP_SERVER_HPP
