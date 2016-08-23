#ifndef _CONNS_MANAGE_HPP_
#define _CONNS_MANAGE_HPP_

#include <boost/noncopyable.hpp>
#include <map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "aisql_connection.hpp"

namespace aisqlpp {

namespace db_config {
    static const string host = "192.168.1.233";
    static const string user = "v5kf";
    static const string passwd = "v5kf";
    static const string db = "v5_law";
}

enum conn_stat {
    conn_working = 1,   // 忙碌状态
    conn_pending = 2,   // 空闲可用状态
    conn_error   = 3,   // 错误状态
};

class conns_manage: public boost::noncopyable
{
public:
    conns_manage() = delete;

    conns_manage(size_t capacity, const string host, const string user,
                          const string passwd, const string db);
    connection_ptr request_conn();
    connection_ptr try_request_conn(size_t msec);
    void free_conn(connection_ptr conn);
    void conn_check();
    size_t capacity() const { return capacity_; }
    size_t free_count() const { return free_cnt_; }

    static size_t generate_conn_uuid(const conns_manage& mng);

    ~conns_manage() = default;
    
private:

    static boost::condition_variable_any conn_notify;
    static boost::mutex conn_notify_mutex;

    size_t aquired_time_; // 使用计数
    size_t capacity_; // total capacity
    size_t free_cnt_; // free connection available
    std::map<connection_ptr, enum conn_stat> conns_;
};


}

#endif  // _CONNS_MANAGE_HPP_
