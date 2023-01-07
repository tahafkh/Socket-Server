#ifndef __ROUTE_HPP__
#define __ROUTE_HPP__
#include "defs.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>

class RequestHandler;

class Route {
public:
    Route(Method _method, std::string _path, RequestHandler *_handler);
    ~Route();

    bool match(Method, std::string url);

    Response *handle(Request *req);
    
private:
    Method method;
    std::string path;
    RequestHandler *handler;
};

#endif
