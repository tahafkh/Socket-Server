#include "../include/request.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

Request::Request(string _method) {
  if (_method == "GET")
    method = GET;
  if (_method == "POST")
    method = POST;
}

string Request::get_path() { return path; }

void Request::set_path(string _path) { path = _path; }

Method Request::get_method() { return method; }

string Request::get_query_param(string key) { return decode_url(query[key]); }

void Request::set_query_param(string key, string value, bool encode) {
  query[key] = encode ? encode_url(value) : value;
}

string Request::get_body_param(string key) { return decode_url(body[key]); }

void Request::set_body_param(string key, string value, bool encode) {
  body[key] = encode ? encode_url(value) : value;
}

string Request::get_header(string key) { return decode_url(headers[key]); }

void Request::set_header(string key, string value, bool encode) {
  headers[key] = encode ? encode_url(value) : value;
}

string Request::get_session_id() {
  string cookie = get_header("cookie");
  if (cookie == "") return "";
  vector<string> v = split(cookie, ";");
  for (string kv : v) {
    vector<string> k = split(kv, "=");
    if (k[0] == "sessionId") return k[1];
  }
  return "";
}

void Request::log() {
  string log = "";
  log += H + string("------- Request --------") + NC + string("\n");
  log += K + string("Method:\t") + NC + (method ? "POST" : "GET") + string("\n");
  log += K + string("Path:\t") + NC + path + string("\n");
  log += K + string("Headers:") + NC + string("\n");

  for (auto it = headers.begin(); !headers.empty() && it != headers.end(); it++)
    log += "  " + decode_url(it->first) + ": " + decode_url(it->second) + string("\n");

  log += "[ " + K + string("SessionId:\t") + NC + this->get_session_id() + " ]" + string("\n");
  log += K + string("Query:") + NC + string("\n");

  for (auto it = query.begin(); !query.empty() && it != query.end(); it++)
    log += "  " + decode_url(it->first) + ": " + decode_url(it->second) + string("\n");

  log += K + string("Body:") + NC + string("\n");
  for (auto it = body.begin(); !body.empty() && it != body.end(); it++)
    log += "  " + decode_url(it->first) + ": " + decode_url(it->second) + string("\n");
    
  log += H + string("------------------------") + NC + string("\n");
  cerr << log << endl;
}
