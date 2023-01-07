#include "../include/handlers.hpp"

using namespace std;

Response *DefaultHandler::callback(Request *req) {
    Response *res = Response::redirect("/page/index.html");
    return res;
}
