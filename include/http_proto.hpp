#ifndef _HTTP_PROTO_H_
#define _HTTP_PROTO_H_

namespace airobot {

    namespace http_proto {

        namespace header_options {

        	static const std::string request_method("request_method_");     // (GET/POST)
        	static const std::string request_uri("request_uri_");           // 
        	static const std::string http_version("http_version_");		    // HTTP/1.0|HTTP/1.1
        	static const std::string request_body("request_body_");		    // used for post

        	static const std::string host("Host");
        	static const std::string accept("Accept");
        	static const std::string range("Range");
        	static const std::string cookie("Cookie");
        	static const std::string referer("Referer");
        	static const std::string user_agent("User-Agent");
        	static const std::string content_type("Content-Type");
        	static const std::string content_length("Content-Length");
        	static const std::string content_range("Content-Range");
        	static const std::string connection("Connection");
        	static const std::string proxy_connection("Proxy-Connection");
        	static const std::string accept_encoding("Accept-Encoding");
        	static const std::string transfer_encoding("Transfer-Encoding");
        	static const std::string content_encoding("Content-Encoding");

        } // namespace header_options


        namespace status {

            static const std::string ok =
              "HTTP/1.1 200 OK\r\n";
            static const std::string created =
              "HTTP/1.1 201 Created\r\n";
            static const std::string accepted =
              "HTTP/1.1 202 Accepted\r\n";
            static const std::string no_content =
              "HTTP/1.1 204 No Content\r\n";
            static const std::string multiple_choices =
              "HTTP/1.1 300 Multiple Choices\r\n";
            static const std::string moved_permanently =
              "HTTP/1.1 301 Moved Permanently\r\n";
            static const std::string moved_temporarily =
              "HTTP/1.1 302 Moved Temporarily\r\n";
            static const std::string not_modified =
              "HTTP/1.1 304 Not Modified\r\n";
            static const std::string bad_request =
              "HTTP/1.1 400 Bad Request\r\n";
            static const std::string unauthorized =
              "HTTP/1.1 401 Unauthorized\r\n";
            static const std::string forbidden =
              "HTTP/1.1 403 Forbidden\r\n";
            static const std::string not_found =
              "HTTP/1.1 404 Not Found\r\n";
            static const std::string internal_server_error =
              "HTTP/1.1 500 Internal Server Error\r\n";
            static const std::string not_implemented =
              "HTTP/1.1 501 Not Implemented\r\n";
            static const std::string bad_gateway =
              "HTTP/1.1 502 Bad Gateway\r\n";
            static const std::string service_unavailable =
              "HTTP/1.1 503 Service Unavailable\r\n";

        }
        
    }

}

#endif
