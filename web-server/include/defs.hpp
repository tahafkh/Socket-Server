#ifndef __DEFS_HPP__
#define __DEFS_HPP__
#include <string>

#define BUFFER_SIZE 4145152
enum Method
    { 
        GET,
        POST
    };

enum State
    {
        REQ,
        HEADER,
        BODY,
        BODY_HEADER,
        BODY_BODY
    };

const std::string NC = "\033[0;39m";
const std::string K = "\033[1m";
const std::string H = "\033[33;1m";
const std::string G = "\033[32m";
const std::string R = "\033[31m";

const std::string SERVER_NAME = "My Web Server";

#endif
