#include "general.hpp"
#include "co_worker.hpp"
#include "http_server.hpp"

namespace airobot {

co_worker::co_worker(const objects* daemons):
    io_service_(),
    signal_(io_service_, /*SIGINT,*/ SIGTERM),
    timing_wheel_timer_(nullptr),
    daemons_(daemons),
    http_(nullptr)
{
    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));

    return;
}

void co_worker::run() 
{ 
    http_ = daemons_->http_server_;
    assert(http_);

    timing_wheel_timer_ = new deadline_timer (io_service_, 
                                              boost::posix_time::seconds(http_->get_conn_check_spare())); 
    assert(timing_wheel_timer_);

    timing_wheel_timer_->async_wait(boost::bind(&co_worker::timing_wheel_check, this,
                                               boost::asio::placeholders::error));

    io_service_.run(); 
}

void co_worker::signal_handler(const boost::system::error_code& error, 
                               int sig_number)
{
    BOOST_LOG_T(info) << "SIGNAL:" << sig_number << "RECEIVED!" << endl;

    // re-listen again!
    signal_.async_wait(boost::bind(&co_worker::signal_handler, this, _1, _2));
}

void co_worker::timed_cancel_socket(const boost::system::error_code& ec,
                                    boost::asio::deadline_timer* t, ip::tcp::socket* sock)
{
    if (ec == boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(info) << "timed_cancel_socket cancelled!";
        return;
    }

    BOOST_LOG_T(info) << "timed_cancel_socket callback called!";
    sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);

    return;
}

void co_worker::timing_wheel_check(const boost::system::error_code& ec)
{

    BOOST_LOG_T(info) << "timing_wheel_check ...";

    bool notify_flag = false;
    auto front_modi = http_->timing_wheel_[0]; //拷贝出front，而front()只是引用
    for (auto& item: http_->timing_wheel_[0])
    {
        if (front_conn_ptr conn = item.lock() )
        {
            if (conn->touch_time_ + boost::posix_time::seconds(
                                    http_->get_conn_expired_time() - http_->get_conn_check_spare()) 
                <= boost::posix_time::second_clock::local_time()) 
            {
                front_modi.erase(item);

                {
                    // 促使socket的读写出错，然后对端被动关闭socket
                    // 当然这里是清理垃圾连接的最后战场，如果对端恶意不关闭
                    // 连接，那么这里必须强制保证连接会被清理掉
                    conn->sock_shutdown(ip::tcp::socket::shutdown_both);
                    conn->set_stats(conn_error);
                    http_->push_to_remove(conn);
                    notify_flag = true;
                }
            }
        }
        else
        {
            // 出现某些因素，conn已经被释放了的情况
            front_modi.erase(item);
        }
    }

    if (notify_flag)
        http_->conn_notify.notify_one();

    // push，将修改更新后的结果添加到尾部
    {
        // 防止push的时候有插入操作
        boost::lock_guard<boost::mutex> lock(http_->front_conns_mutex_);
        http_->timing_wheel_.push_back(front_modi);
    }


    // 设定一个时间间隔。由于检测需要时间，总体的时间会不准确，
    // 但不是关键业务，可以忽略误差
    timing_wheel_timer_->expires_from_now(
                boost::posix_time::seconds(http_->get_conn_check_spare()));
    timing_wheel_timer_->async_wait(boost::bind(&co_worker::timing_wheel_check, this,
                                               boost::asio::placeholders::error));

    return;
}

}

