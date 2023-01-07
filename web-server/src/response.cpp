#include "../include/response.hpp"
#include "../include/defs.hpp"
#include <cstring>
#include <iostream>
#include <map>

using namespace std;

map<int, string> get_http_phrases() {
  map<int, string> http_phrases;
  http_phrases[200] = "OK";
  http_phrases[303] = "See Other";
  http_phrases[404] = "Not Found";
  http_phrases[500] = "Internal Server Error";
  return http_phrases;
}

map<int, string> http_phrases = get_http_phrases();

Response::Response(int code) {
  this->set_code(code);
  this->headers["Content-Type"] = "text/plain";
}

void Response::set_header(string name, string value) { headers[name] = value; }

void Response::set_body(string _body) { body = _body; }

void Response::set_code(int _code) {
  code = _code;
  phrase = http_phrases[code];
}

void Response::set_session_id(string session_id) {
  set_header("set-cookie", "sessionId=" + session_id + ";");
}

Response *Response::redirect(string url) {
  Response *res = new Response(303);
  res->set_header("Location", url);
  return res;
}

string Response::show(int &size) {
  string header = "";
  header += "HTTP/1.0 " + to_string(code) + " " + phrase + "\r\n";
  header += "Server: " + SERVER_NAME + " \r\n";
  header += "Content-Length: " + to_string(body.size()) + "\r\n";
  for (auto it = headers.begin(); !headers.empty() && it != headers.end(); it++)
    header += it->first + ": " + it->second + "\r\n";
  header += "\r\n";
  size = header.size() + body.size();
  return header + body;
}

void Response::log(bool show_body) {
  string log = "";
  log += H + string("------- Response -------") + NC + string("\n");
  log += K + string("Status:\t") + NC + (code == 200 ? G : R) + to_string(code) + " " + phrase + NC + string("\n");
  log += K + string("Headers:") + NC + string("\n");

  for (auto it = headers.begin(); !headers.empty() && it != headers.end(); it++)
    log += "  " + decode_url(it->first) + ": " + decode_url(it->second) + string("\n");

  if (show_body) log += K + string("Body:\n") + NC + body + string("\n");
  
  log += H + string("------------------------") + NC + string("\n");
  cerr << log << endl;
}
