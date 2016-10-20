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
            }
        }

        return *redis_instance_;
   }

private:
   static void createInstance()
   {
        static RedisClient theInstance;

        BOOST_LOG_T(info) << "Generating RedisClient...";
        BOOST_ASSERT(!context_);
        context_ = redisConnect("127.0.0.1", 6379);
        if (context_ != NULL && context_->err) {
            BOOST_LOG_T(error) << "Redis Error:" << context_->errstr;
            ::abort();
        }

        redis_instance_ = &theInstance;
        boost::atomic_thread_fence(boost::memory_order_release);
        return;
   }

   template <typename T>
   bool setValue(const std::string& key, const T& val);
   template <typename T>
   bool getValue(const std::string& key, T& val);

   //声明但是永远不会使用
   ~RedisClient() { redisFree(context_); }
   RedisClient() {}
   RedisClient(const RedisClient&);
   RedisClient& operator=(const RedisClient&);

private:
   // will defined at front_conn.cpp
   static RedisClient* redis_instance_;
   static boost::mutex redis_lock_;
   static redisContext* context_;
};


template <>
bool RedisClient::setValue(const std::string& key, const std::string& val)
{
   boost::lock_guard<boost::mutex> lock(redis_lock_);
   std::string cmd = (boost::format("GET %s %s") % key % val).str();
   redisReply* reply = static_cast<redisReply *> ( redisCommand(context_, cmd.c_str()) );

   return true;
}

template <>
bool RedisClient::getValue(const std::string& key, std::string& val)
{
   boost::lock_guard<boost::mutex> lock(redis_lock_);
   std::string cmd = (boost::format("GET %s") % key).str();
   redisReply* reply = static_cast<redisReply *> ( redisCommand(context_, cmd.c_str()) );

   if (reply->type != REDIS_REPLY_STRING)
   {
       BOOST_LOG_T(error) << "Reply error!";
       return false;
   }

   val = std::string(reply->str);
   BOOST_LOG_T(info) << val;
   return true;
}


}

#endif
