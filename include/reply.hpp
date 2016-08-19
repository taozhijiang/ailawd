#ifndef _REPLY_H_
#define _REPLY_H_

#include "general.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace airobot {

struct header
{
  std::string name;
  std::string value;
};


static const char header_name_value_separator[] = { ':', ' ' };
static const char header_crlf[] = { '\r', '\n'};

static string header_name_value_separator_str = ": ";
static string header_crlf_str = "\r\n";

class reply
{
public:
    reply() = delete;

    static std::string fixed_reply_error;
    static std::string fixed_reply_ok;

    static string reply_generate(const string& content, const string& stat_str)
    {
        std::vector<header> headers(5);

        // reply fixed header
        headers[0].name = "Server";
        headers[0].value = "ailawd/1.0";
        headers[1].name = "Date";
        headers[1].value = to_simple_string(second_clock::universal_time());
        headers[2].name = "Content-Length";
        headers[2].value = std::to_string((long long unsigned)content.size()+1);
        headers[3].name = "Content-Type";
        headers[3].value = "text/html";
        headers[4].name = "Connection";
        headers[4].value = "keep-alive";

        string str = stat_str;
        for (int i=0; i< headers.size(); ++i)
        {
            str += headers[i].name;
            str += header_name_value_separator_str;
            str += headers[i].value;
            str += header_crlf_str;
        }
        
        str += header_crlf_str;
        str += content;

        return str;
    }
};

}

#endif
