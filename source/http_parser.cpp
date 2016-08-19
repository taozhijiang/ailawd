#include "general.hpp"
#include "http_parser.hpp"
#include "http_proto.hpp"

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <boost/regex.hpp>

namespace airobot {
    
    bool http_parser::parse_request(const char* ptr)
    {
        if (!ptr || !strlen(ptr) || !strstr(ptr, "\r\n\r\n"))
        {
            std::cerr << "Check header package..." << std::endl;
            return false;
        }
        
        std:string header = std::string(ptr);
        assert(boost::find_first(header, "\r\n\r\n")); //boost algo
        header_opts_.clear();

        header_opts_.insert(std::make_pair(http_proto::header_options::request_body, 
                                          header.substr(header.find("\r\n\r\n")+4))); 
        header = header.substr(0, header.find("\r\n\r\n")+4);

        std::istringstream resp(header);
        std::string item;
        std::string::size_type index;

        while (std::getline(resp, item) && item != "\r") 
        {
            index = item.find(':', 0);
            if(index != std::string::npos) 
            {
                header_opts_.insert(std::make_pair(
                        boost::algorithm::trim_copy(item.substr(0, index)), 
                        boost::algorithm::trim_copy(item.substr(index + 1)) ));
            }
            else
            {
                boost::smatch what;
                if (boost::regex_match(item, what,
					boost::regex("([a-zA-Z]+)[ ]+([^ ]+)([ ]+(.*))?")))
                {
                    header_opts_.insert(std::make_pair(http_proto::header_options::request_method, 
                                                       boost::algorithm::trim_copy(boost::to_upper_copy(string(what[1]))))); 
                    header_opts_.insert(std::make_pair(http_proto::header_options::request_uri, 
                                                       boost::algorithm::trim_copy(string(what[2])))); 
                    header_opts_.insert(std::make_pair(http_proto::header_options::http_version, 
                                                       boost::algorithm::trim_copy(string(what[3])))); 
                }
            }
        }

        return true;
    }

    std::string http_parser::request_option(const std::string option_name)
    {
        if (!option_name.size())
            return "";
        
        for (auto &item : header_opts_)
        {
            if (boost::iequals(option_name, item.first)) 
                return item.second;
        }
        
        return "";
    }

}
