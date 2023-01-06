#include "../include/route.hpp"
#include "../include/server.hpp"

using namespace std;

Route::Route(Method _method, string _path) {
  method = _method;
  path = _path;
}

void Route::set_handler(RequestHandler *_handler) { handler = _handler; }

bool Route::match(Method _method, string url) {
  return (url == path) && (_method == method);
}

Response *Route::handle(Request *req) { return handler->callback(req); }

Route::~Route() { delete handler; }
