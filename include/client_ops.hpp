#ifndef _CLIENT_OPS_H_
#define _CLIENT_OPS_H_

#include "general.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

/**
 * 主要提供一些客户端的同步操作
 */

namespace airobot {

class client_ops
{

public:

    // 连接成功返回true
    void sync_timed_connect(class co_worker* p_work, 
                            const ip::tcp::endpoint& ep, ip::tcp::socket& sock, size_t time_ms,
                            boost::system::error_code &ec);

    // 同步读取操作接口
    // 这里采用了一个约定，就是以 字符串表示的实体长度+':'+实体　的格式来传递
    // 后续会预留前四个字节用网络序表示长度

    size_t sync_read_from(ip::tcp::socket& sock, std::vector<char>& read_buff /*FIX ME*/,
                          boost::system::error_code &ec);
    size_t sync_timed_read_from(class co_worker* p_work, 
                                ip::tcp::socket& sock, std::vector<char>& read_buff /*FIX ME*/, 
                                size_t time_ms, boost::system::error_code &ec);

    // 下面提供一些简单的接口，一次调用read_some，在boost::asio
    // 默认大概支持1.5K，类似一个MTU
    size_t sync_read_some(ip::tcp::socket& sock, std::vector<char>& read_buff,
                          boost::system::error_code &ec);
    size_t sync_timed_read_some(class co_worker* p_work, 
                                ip::tcp::socket& sock, std::vector<char>& read_buff, 
                                size_t time_ms, boost::system::error_code &ec);
};

}


#endif //_CLIENT_OPS_H_
