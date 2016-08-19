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
#define BOOST_LOG_T(x) BOOST_LOG_TRIVIAL(x)<<::basename(__FILE__)<<":"<<__LINE__<<"[@"<<__func__<<"]"<<" " 

}



#endif // _GENERAL_H_
