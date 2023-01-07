#include "../include/route.hpp"
#include "../include/server.hpp"

using namespace std;

Route::Route(Method _method, string _path, RequestHandler *_handler) {
  method = _method;
  path = _path;
  handler = _handler;
}

bool Route::match(Method _method, string url) {
  return (url == path) && (_method == method);
}

Response *Route::handle(Request *req) { return handler->callback(req); }

Route::~Route() { delete handler; }
