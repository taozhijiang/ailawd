#ifndef _CONNS_MANAGE_HPP_
#define _CONNS_MANAGE_HPP_

#include <boost/noncopyable.hpp>
#include <boost/container/map.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "aisql_connection.hpp"

namespace aisqlpp {

namespace db_config {
    static const string host = "127.0.0.1";
    static const string user = "v5kf";
    static const string passwd = "v5kf";
    static const string db = "v5_law";
}

enum conn_stat {
    conn_working = 1,   // 忙碌状态
    conn_pending = 2,   // 空闲可用状态
    conn_error   = 3,   // 错误状态
};


struct connection_ptr_compare {
    bool operator() (const aisqlpp::connection_ptr& lhs, 
                        const aisqlpp::connection_ptr& rhs) const
    {
        return (lhs.get() < rhs.get());
    }
};

class conns_manage: public boost::noncopyable
{
public:
    conns_manage() = delete;

    conns_manage(size_t capacity, const string host, const string user,
                          const string passwd, const string db);

    // 由于会返回nullptr，所以不能返回引用
    connection_ptr request_conn();
    connection_ptr try_request_conn(size_t msec);
    bool request_scoped_conn(connection_ptr& conn);
    void free_conn(connection_ptr& conn);
    void conn_check();
    void conn_details();
    size_t conn_capacity() const { return capacity_; }
    size_t free_count() const { return free_cnt_; }

    enum conn_stat get_conn_stat(const connection_ptr& conn);
    static size_t generate_conn_uuid(const conns_manage& mng);

    ~conns_manage() = default;
    
private:

    boost::condition_variable_any conn_notify;
    boost::mutex conn_notify_mutex;

    size_t aquired_time_; // 使用计数
    size_t capacity_; // total capacity
    size_t free_cnt_; // free connection available

    boost::container::map<connection_ptr, enum conn_stat, connection_ptr_compare> conns_;
};


}

#endif  // _CONNS_MANAGE_HPP_
