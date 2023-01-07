#ifndef __HANDLERS_HPP__
#define __HANDLERS_HPP__
#include "server.hpp"

class DefaultHandler : public RequestHandler {
public:
    Response *callback(Request *);
};

#endif
