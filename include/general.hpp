#ifndef _GENERAL_H_
#define _GENERAL_H_

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <string>
using std::string;

#include <cstdint>
using std::int64_t;
using std::uint64_t;

#include <boost/asio.hpp>
using namespace boost::asio;

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp> 

#include <boost/bind.hpp>

#include <boost/log/trivial.hpp>

namespace airobot {

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

extern char *basename(char *path);
#if 1
#define BOOST_LOG_T(x) BOOST_LOG_TRIVIAL(x)<<::basename(__FILE__)<<":"<<__LINE__<<"[@"<<__func__<<"]"<<" " 
#else
#define BOOST_LOG_T(x) std::cerr<<std::endl<<#x<<::basename(__FILE__)<<":"<<__LINE__<<"[@"<<__func__<<"]"<<" " 
#endif

class http_server;
class co_worker;

/**
 * 这里面的对象都是常驻对象，所以这里没有使用智能指针了 
 * 用一个object进行封装，主要是添加协作类方便点，不用互相 
 * 引用改变个各类的代码 
 */
class objects {
public:
    http_server* http_server_;
    co_worker  * co_worker_;
};

}



#endif // _GENERAL_H_
