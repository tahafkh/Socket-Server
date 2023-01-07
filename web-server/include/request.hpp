#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__
#include "defs.hpp"
#include "utilities.hpp"
#include <string>

class Request {
public:
    Request(std::string method="GET");

    std::string get_path();
    void set_path(std::string);

    Method get_method();

    std::string get_query_param(std::string key);
    void set_query_param(std::string key, std::string value, bool encode=true);

    std::string get_body_param(std::string key);
    void set_body_param(std::string key, std::string value, bool encode=true);

    std::string get_header(std::string key);
    void set_header(std::string key, std::string value, bool encode=true);

    std::string get_session_id();

    void log();

private:
    std::string path;
    Method method;
    csmap headers;
    csmap query;
    csmap body;
};

#endif
