#ifndef __RESPONSE_HPP__
#define __RESPONSE_HPP__
#include "../include/defs.hpp"
#include "../include/utilities.hpp"
#include <string>

class Response{
public:
    Response(int code=200);

    void set_header(std::string name, std::string value);

    void set_body(std::string _body);

    void set_session_id(std::string session_id);

    static Response *redirect(std::string url);

    std::string show(int &);
    void log(bool show_body=false);

private:
    int code;
    std::string phrase;
    std::string body;
    csmap headers;
};

#endif