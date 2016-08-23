#ifndef _CONNECTION_HPP_
#define _CONNECTION_HPP_

#include "aisql_general.hpp"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace aisqlpp {

class conns_manage;

class connection;
using connection_ptr = boost::shared_ptr<connection>;

class connection final: public boost::noncopyable
{
public:
    connection() = delete;
    connection(conns_manage& manage, size_t conn_uuid,
               string host, string user, string passwd, string db);
    ~connection();
    void set_uuid(size_t uuid) { conn_uuid_ = uuid; }
    size_t get_uuid() { return conn_uuid_; }

    /**
     * SQL API
     */
    bool execute_command(const string& sql);
    bool execute_query(const string& sql);
    size_t execute_query_count(const string& sql);
    bool execute_check_exist(const string& sql);
    // 不会修改内部指针引用计数
    sql::ResultSet* get_result_set() { return result_.get(); }

    template<typename T>
    bool execute_query_column(const string& sql, std::vector<T>& vec);
    template<typename T>
    bool execute_query_value(const string& sql, T& val);
    bool execute_query_column(const string& sql, std::vector<std::string>& vec);
    bool execute_query_value(const string& sql, std::string& val);

private:
    sql::Driver* driver_;

    size_t  conn_uuid_; // RAND_MAX
    boost::shared_ptr<sql::Connection>  conn_;
    boost::shared_ptr<sql::Statement>   stmt_;

    // 因为都是获取连接之后查询，然后释放连接，所以这里
    // 是线程安全的，在下一次查询需要使用result_set的时候
    // 先进行reset()就可以清空之前的查询数据集，接收新的结果
    // 所以这里不会导致内存泄漏
    boost::shared_ptr<sql::ResultSet>   result_;

    // prep_stmt_ create manually
    // boost::shared_ptr< sql::PreparedStatement > prep_stmt_;

    // may be used future
    conns_manage&    manage_;
};

template<typename T>
bool connection::execute_query_column(const string& sql, std::vector<T>& vec)
{
    try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        if (result_->rowsCount() == 0)
            return false;

        vec.clear();
        while (result_->next()) 
        {
            if (typeid(T) == typeid(float) ||
                typeid(T) == typeid(double) )
            {
                vec.push_back(static_cast<T>(result_->getDouble(1)));
            }
            else if (typeid(T) == typeid(int) ||
                typeid(T) == typeid(int64_t) )
            {
                vec.push_back(static_cast<T>(result_->getInt64(1)));
            }
            else if (typeid(T) == typeid(unsigned int) ||
                typeid(T) == typeid(uint64_t) )
            {
                vec.push_back(static_cast<T>(result_->getUInt64(1)));
            }
            else
            {
                BOOST_LOG_T(error) << "Unsupported type: " << typeid(T).name() << endl;
            }
        }

        return true;

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " STMT: " << sql << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}

template<typename T>
bool connection::execute_query_value(const string& sql, T& val)
{
        try {

        if(!conn_->isValid()) 
            conn_->reconnect();

        stmt_->execute(sql);
        result_.reset(stmt_->getResultSet());
        if (result_->rowsCount() == 0)
            return false;

        if (result_->rowsCount() != 1) 
        {
            BOOST_LOG_T(error) << "Error rows count:" << result_->rowsCount() << endl;
            return false;
        }

        while (result_->next()) 
        {
            if (typeid(T) == typeid(float) ||
                typeid(T) == typeid(double) )
            {
                val = static_cast<T>(result_->getDouble(1));
            }
            else if (typeid(T) == typeid(int) ||
                typeid(T) == typeid(int64_t) )
            {
                val = static_cast<T>(result_->getInt64(1));
            }
            else if (typeid(T) == typeid(unsigned int) ||
                typeid(T) == typeid(uint64_t) )
            {
                val = static_cast<T>(result_->getUInt64(1));
            }
            else
            {
                BOOST_LOG_T(error) << "Unsupported type: " << typeid(T).name() << endl;
            }
        }

        return true;

    } catch (sql::SQLException &e) 
    {
        BOOST_LOG_T(error) << " STMT: " << sql << endl;
        BOOST_LOG_T(error) << "# ERR: " << e.what() << endl;
        BOOST_LOG_T(error) << " (MySQL error code: " << e.getErrorCode() << endl;
        BOOST_LOG_T(error) << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }
}

}

#endif  // _CONNECTION_HPP_
