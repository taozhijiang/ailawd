#include "general.hpp"

#include "client_ops.hpp"
#include "co_worker.hpp"


namespace airobot {

void client_ops::sync_timed_connect(class co_worker* p_work, 
                                    const ip::tcp::endpoint& ep, ip::tcp::socket& sock, size_t time_ms,
                                    boost::system::error_code &ec)
{
    if (time_ms == 0)
    {
        sock.connect(ep, ec);
        return;
    }

    boost::asio::deadline_timer timer(p_work->get_io_service(), 
                                      boost::posix_time::milliseconds(time_ms));

    timer.async_wait(boost::bind(&co_worker::timed_cancel_socket, p_work,
                                 boost::asio::placeholders::error, &timer, &sock));

    sock.connect(ep, ec);

    if(timer.expires_at() > deadline_timer::traits_type::now())
    {
        BOOST_LOG_T(info) << "Cancel Connect Timer here!";
        timer.cancel();
    }
}

// 返回总共有效的负载结果，返回0表示错误
size_t client_ops::sync_read_from(ip::tcp::socket& sock, std::vector<char>& read_buff /*FIX ME*/,
                      boost::system::error_code &ec)
{
    size_t len = sock.read_some(buffer(read_buff), ec);
    if (ec)
    {
        BOOST_LOG_T(error) << "READ from server failed!";
        return 0;
    }

    string len_str = string(string(read_buff.data(), 20).c_str());
    size_t pos = len_str.find(':');
    if(pos == std::string::npos)
    {
        BOOST_LOG_T(error) << " Parse head from server failed!";
        return 0;
    }

    string tmp_str = len_str.substr(0, pos);
    size_t real_len = ::atoi(tmp_str.c_str());
    if (!real_len)
    {
        BOOST_LOG_T(error) << " Parse head from server failed!";
        return 0;
    }

    // erase the front
    read_buff.erase(read_buff.begin(), read_buff.begin() + pos + 1);

    // relarge the receive buffer
    if (real_len > read_buff.size())
    {
        BOOST_LOG_T(info) << " Relarge the receive buff to size: " << (real_len + 256);
        read_buff.resize(real_len + 256);
    }

    size_t actual_len = len - pos - 1;
    while (actual_len < real_len)
    {
        len = sock.read_some(buffer(read_buff.data() + actual_len, 
                                            read_buff.size() - actual_len), ec);
        if (ec || len == 0)
        {
            BOOST_LOG_T(error) << "sub READ from server failed!";
            return actual_len; 
        }
        actual_len += len;
    }

    return actual_len;
}

// 返回总共有效的负载结果，返回0表示错误
size_t client_ops::sync_timed_read_from(class co_worker* p_work, 
                            ip::tcp::socket& sock, std::vector<char>& read_buff /*FIX ME*/, 
                            size_t time_ms, boost::system::error_code &ec)
{
    size_t ret = 0;

    if (time_ms == 0)
        return sync_read_from(sock, read_buff, ec);
 
    boost::asio::deadline_timer timer(p_work->get_io_service(), 
                                      boost::posix_time::milliseconds(time_ms));

    timer.async_wait(boost::bind(&co_worker::timed_cancel_socket, p_work,
                                 boost::asio::placeholders::error, &timer, &sock));

    ret = sync_read_from(sock, read_buff, ec);

    if(timer.expires_at() > deadline_timer::traits_type::now())
    {
        BOOST_LOG_T(info) << "Cancel Read From Timer here!";
        timer.cancel();
    }

    return ret;
}


size_t client_ops::sync_read_some(ip::tcp::socket& sock, std::vector<char>& read_buff,
                      boost::system::error_code &ec)
{
    size_t len = sock.read_some(buffer(read_buff), ec);
    if (ec)
    {
        BOOST_LOG_T(error) << "READ from server failed!";
        return 0;
    }

    return len;
}

size_t client_ops::sync_timed_read_some(class co_worker* p_work, 
                            ip::tcp::socket& sock, std::vector<char>& read_buff,
                            size_t time_ms, boost::system::error_code &ec)
{
    size_t ret = 0;

    if (time_ms == 0)
        return sync_read_some(sock, read_buff, ec);
 
    boost::asio::deadline_timer timer(p_work->get_io_service(), 
                                      boost::posix_time::milliseconds(time_ms));

    timer.async_wait(boost::bind(&co_worker::timed_cancel_socket, p_work,
                                 boost::asio::placeholders::error, &timer, &sock));

    ret = sync_read_some(sock, read_buff, ec);

    if(timer.expires_at() > deadline_timer::traits_type::now())
    {
        BOOST_LOG_T(info) << "Cancel Read Some Timer here!";
        timer.cancel();
    }

    return ret;
}

}
