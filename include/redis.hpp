#ifndef _REDIS_H_
#define _REDIS_H_

#include <boost/noncopyable.hpp>
#include <boost/atomic.hpp>
#include <boost/format.hpp>

#include <hiredis/hiredis.h>

#include "general.hpp"

namespace airobot {

// 单例模式的Redis客户端

#if 0
class RedisClient:public boost::noncopyable
{
public:
   RedisClient& getInstance()
   {
        RedisClient* tmp = redis_instance_.load(boost::memory_order_relaxed);
        boost::atomic_thread_fence(boost::memory_order_acquire);
        if (!tmp)
        {
            boost::lock_guard<boost::mutex> lock(redis_lock_);
            tmp = redis_instance_.load(boost::memory_order_relaxed);
            if (!tmp)
            {
                tmp = createInstance();
                boost::atomic_thread_fence(boost::memory_order_release);
                redis_instance_.store(tmp, boost::memory_order_relaxed);
            }
        }

        return *tmp;
   }

private:
   static RedisClient* createInstance()
   {
       std::cerr << "Generating RedisClient..." << std::endl;
       static RedisClient theInstance;
       return &theInstance;
   }

   //声明但是永远不会使用
   ~RedisClient();
   RedisClient();
   RedisClient(const RedisClient&);
   RedisClient& operator=(const RedisClient&);

private:
   // will defined at front_conn.cpp
   static boost::atomic<RedisClient*> redis_instance_;
   static boost::mutex redis_lock_;
};
#endif

class RedisClient:public boost::noncopyable
{
public:
    static RedisClient& getInstance()
    {
        if (!redis_instance_)
        {
            boost::lock_guard<boost::mutex> lock(redis_lock_);
            boost::atomic_thread_fence(boost::memory_order_acquire);
            if (!redis_instance_)
            {
                createInstance();
                redis_instance_->initContext();
            }
        }

        return *redis_instance_;
    }

    // general support longlong, string
    template <typename K, typename T>
    bool setValue(const K& key, const T& val);
    template <typename K, typename T>
    bool getValue(const K& key, T& val);

    /**
     * binary compatible protocol
     */
    template <typename T>
    bool setValue(unsigned char* key, size_t key_len, const T& val);
    template <typename T>
    bool getValue(unsigned char* key, size_t key_len, T& val);

private:
    static void createInstance()
    {
        static RedisClient theInstance;
        BOOST_LOG_T(info) << "Generating RedisClient...";

        redis_instance_ = &theInstance;
        boost::atomic_thread_fence(boost::memory_order_release);
        return;
    }

    void initContext()
    {
        if(context_)
        {
            redisFree(context_);
            context_ = NULL;
        }

        struct timeval timeout = { 5, 0 };  //5s
        context_ = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
        if (context_ != NULL && context_->err) {
            if (context_)
            {
                BOOST_LOG_T(error) << "Redis Connect Error:" << context_->errstr;
                redisFree(context_);
                context_ = NULL;
            }
            else
                BOOST_LOG_T(error) << "Connection error: can't allocate redis context!";
        }
        else
            BOOST_LOG_T(info) << "Connect to redis server 127.0.0.1:6379 OK!" ;

        return;
    }

     //声明但是永远不会使用
    ~RedisClient() { redisFree(context_); }
    RedisClient() {}
    RedisClient(const RedisClient&);
    RedisClient& operator=(const RedisClient&);

private:
    // will defined at front_conn.cpp
    static RedisClient* redis_instance_;
    static boost::mutex redis_lock_;

    redisContext* context_;
};


template <>
bool RedisClient::setValue(const std::string& key, const std::string& val)
{
    boost::lock_guard<boost::mutex> lock(redis_lock_);
    redisReply* reply = static_cast<redisReply *> (
                redisCommand(context_, "SET %s %s", key.c_str(), val.c_str()) );

    // 断开重连
    if (!reply)
    {
        BOOST_LOG_T(error) << "setValue with error:" << context_->errstr;
        initContext();
        return false;
    }

    if (reply->type != REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK"))
    {
        BOOST_LOG_T(error) << "Redis reply error with:" << reply->type << ": " << reply->str;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    return true;
}

template <>
bool RedisClient::getValue(const std::string& key, std::string& val)
{
    boost::lock_guard<boost::mutex> lock(redis_lock_);
    redisReply* reply = static_cast<redisReply *> (
                redisCommand(context_, "GET %s", key.c_str()) );

    // 断开重连
    if (!reply)
    {
        BOOST_LOG_T(error) << "setValue with error:" << context_->errstr;
        initContext();
        return false;
    }

    if (reply->type != REDIS_REPLY_STRING)
    {
        if (reply->type != REDIS_REPLY_NIL )
            BOOST_LOG_T(error) << "Redis reply error with:" << reply->type;

        freeReplyObject(reply);
        return false;
    }

    val = std::string(reply->str);

    freeReplyObject(reply);
    return true;
}


template <>
bool RedisClient::setValue(unsigned char* key, size_t key_len, const std::string& val)
{
    boost::lock_guard<boost::mutex> lock(redis_lock_);
    redisReply* reply = static_cast<redisReply *> (
                redisCommand(context_, "SET %b %s", key, key_len, val.c_str()) );

    // 断开重连
    if (!reply)
    {
        BOOST_LOG_T(error) << "setValue with error:" << context_->errstr;
        initContext();
        return false;
    }

    if (reply->type != REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK"))
    {
        BOOST_LOG_T(error) << "Redis reply error with:" << reply->type << ": " << reply->str;
        freeReplyObject(reply);
        return false;
    }

    freeReplyObject(reply);
    return true;
}

template <>
bool RedisClient::getValue(unsigned char* key, size_t key_len, std::string& val)
{
    boost::lock_guard<boost::mutex> lock(redis_lock_);
    redisReply* reply = static_cast<redisReply *> (
                redisCommand(context_, "GET %b", key, key_len) );

    // 断开重连
    if (!reply)
    {
        BOOST_LOG_T(error) << "setValue with error:" << context_->errstr;
        initContext();
        return false;
    }

    if (reply->type != REDIS_REPLY_STRING)
    {
        if (reply->type != REDIS_REPLY_NIL )
            BOOST_LOG_T(error) << "Redis reply error with:" << reply->type;

        freeReplyObject(reply);
        return false;
    }

    val = std::string(reply->str);

    freeReplyObject(reply);
    return true;
}

}

#endif
