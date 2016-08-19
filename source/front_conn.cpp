#include "front_conn.hpp"
#include "reply.hpp"
#include "http_server.hpp"
#include "json11.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace airobot {

front_conn::front_conn(boost::shared_ptr<ip::tcp::socket> p_sock,
                       http_server& server):
    connection(p_sock),
    parser_(),
    server_(server)
{
    p_buffer_ = boost::make_shared<std::vector<char> >(16*1024, 0);
    p_write_  = boost::make_shared<std::vector<char> >(16*1024, 0); 
}

void front_conn::stop()
{
    //server_.set_session_id(shared_from_this(), (int64_t)-1);
    set_stats(conn_pending);
}


void front_conn::read_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred) 
    {
        //cout << &(*p_buffer_)[0] << endl;

        // read more
        //string ret = reply::reply_generate(string(&(*p_buffer_)[0]), http_proto::status::ok); 
        //string ret = reply::reply_generate("<html><body><p>Hello World!</p></body></html>");

        if( parser_.parse_request(p_buffer_->data()) )
        {
            string body = parser_.request_option(http_proto::header_options::request_body);
            if (body != "")
            {
                string json_err;
                auto json_parsed = json11::Json::parse(body, json_err);

                // TO DO 
            }
        }
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "READ ERROR FOUND!";
        notify_conn_error();
        return;
    }

    memcpy(p_write_->data(), 
           reply::fixed_reply_error.c_str(), 
           reply::fixed_reply_error.size()+1);

ok_return:
    do_write();

ok_no_return:
    do_read();

    return;
}


void front_conn::write_handler(const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (!ec && bytes_transferred) 
    {
        //cout << "WRITE OK!" << endl;

        //不会主动调读的
        //do_write();
    }
    else if (ec != boost::asio::error::operation_aborted)
    {
        BOOST_LOG_T(error) << "WRITE ERROR FOUND!";
        notify_conn_error();
    }
}


void front_conn::notify_conn_error()
{
    {
        boost::lock_guard<boost::mutex> lock(server_.conn_notify_mutex); 
        p_sock_->close();
        set_stats(conn_error);
    }
    server_.conn_notify.notify_one();
}

}
