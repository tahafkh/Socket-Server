#include "../include/server.hpp"
#include "../include/utilities.hpp"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 // win xp
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
// POSIX sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#endif

static const char *get_socket_error()
{
#ifdef _WIN32
    static char message[256];
    message[0] = '\0';
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, WSAGetLastError(), 0, (LPSTR)&message, sizeof(message), NULL);
    char *newline = strrchr(message, '\n');
    if (newline)
        *newline = '\0';
    return message;
#else
    return strerror(errno);
#endif
}

using namespace std;

class NotFoundHandler : public ShowPage
{
public:
    NotFoundHandler(string _path) : ShowPage(_path) {}
    Response *callback(Request *req);
};

Response *NotFoundHandler::callback(Request *req)
{
    Response *res = ShowPage::callback(req);
    res->set_code(404);
    return res;
}

class ServerErrorHandler
{
public:
    static Response *callback(string msg);
};

Response *ServerErrorHandler::callback(string msg)
{
    Response *res = new Response(500);
    res->set_header("Content-Type", "application/json");
    res->set_body("{ \"code\": \"500\", \"message\": \"" + msg + "\" }\n");
    return res;
}

void split(string str, string separator, int max, vector<string> &results)
{
    int i = 0;
    size_t found = str.find_first_of(separator);

    while (found != string::npos)
    {
        if (found > 0)
            results.push_back(str.substr(0, found));
        str = str.substr(found + 1);
        found = str.find_first_of(separator);

        if (max > -1 && ++i == max)
            break;
    }

    if (str.length() > 0)
        results.push_back(str);
}

void check_data(char *raw_headers, size_t length) {
    for (size_t i = 0; i < length; i++)
    {
        if (!raw_headers[i])
            throw Server::Exception("Unsupported binary data in request.");
    }
}

size_t calculate_real_body_size(char *raw_headers){
    return string(raw_headers).size() -
            split(string(raw_headers), "\r\n\r\n", false)[0].size() -
            string("\r\n\r\n").size();
}

Request *handle_req_state(string line, State &state) {
    vector<string> R = split(line, " ", false);
    if (R.size() != 3)
    {
        throw Server::Exception("Invalid header (request line)");
    }
    auto req = new Request(R[0]);
    req->set_path(R[1]);
    size_t pos = req->get_path().find('?');
    if (pos != string::npos && pos != req->get_path().size() - 1)
    {
        vector<string> Q1 = split(req->get_path().substr(pos + 1), "&", false);
        for (vector<string>::size_type q = 0; q < Q1.size(); q++)
        {
            vector<string> Q2 = split(Q1[q], "=", false);
            if (Q2.size() == 2)
                req->set_query_param(Q2[0], Q2[1], false);
            else
                throw Server::Exception("Invalid query");
        }
    }
    req->set_path(req->get_path().substr(0, pos));
    state = HEADER;
    return req;
}

State handle_header_state(string line, Request *req, size_t real_body_size, string &boundary) {
    if (req->get_header("Content-Type")
            .substr(0, string("multipart/form-data").size()) ==
        "multipart/form-data")
    {
        boundary =
            req->get_header("Content-Type")
                .substr(req->get_header("Content-Type").find("boundary=") +
                        string("boundary=").size());
    }
    return BODY;
}

State handle_body_state(Request *req, string line, string boundary, bool &should_be_empty,
                        string &last_field_key, string &last_field_value) {
    State state = BODY;
    if (req->get_header("Content-Type") == "")
    {
    }
    else if (req->get_header("Content-Type") ==
                "application/x-www-form-urlencoded")
    {
        vector<string> body = split(line, "&", false);
        for (size_t i = 0; i < body.size(); i++)
        {
            vector<string> field = split(body[i], "=", false);
            if (field.size() == 2)
                req->set_body_param(field[0], field[1], false);
            else if (field.size() == 1)
                req->set_body_param(field[0], "", false);
            else
                throw Server::Exception("Invalid body");
        }
    }
    else if (req->get_header("Content-Type")
                    .substr(0, string("multipart/form-data").size()) ==
                "multipart/form-data")
    {
        if (line == "--" + boundary || line == "--" + boundary + "--")
        {
            last_field_key = "";
            last_field_value = "";
            should_be_empty = false;
            state = BODY_HEADER;
        }
    }
    else
    {
        throw Server::Exception("Unsupported body type: " +
                                req->get_header("Content-Type"));
    }
}

void handle_body_header_state(string line, string &last_field_key, bool &should_be_empty) {
    vector<string> R = split(line, ": ", false);
    if (R.size() != 2)
        throw Server::Exception("Invalid header");
    if (lowercase(R[0]) == lowercase("Content-Disposition"))
    {
        vector<string> A = split(R[1], "; ", false);
        for (size_t i = 0; i < A.size(); i++)
        {
            vector<string> attr = split(A[i], "=", false);
            if (attr.size() == 2)
            {
                if (lowercase(attr[0]) == lowercase("name"))
                {
                    last_field_key = attr[1].substr(1, attr[1].size() - 2);
                }
            }
            else if (attr.size() == 1)
            {
            }
            else
                throw Server::Exception("Invalid body attribute");
        }
    }
    else if (lowercase(R[0]) == lowercase("Content-Type"))
    {
        if (lowercase(R[1]) == lowercase("application/octet-stream"))
            should_be_empty = true;
        else if (lowercase(R[1].substr(0, R[1].find("/"))) !=
                    lowercase("text"))
            throw Server::Exception("Unsupported file type: " + R[1]);
    }
}

State handle_body_body_state(Request *req, string line, string boundary, bool &should_be_empty,
                            string &last_field_key, string &last_field_value) {
    State state = BODY_BODY;
    if (line == "--" + boundary || line == "--" + boundary + "--")
    {
        req->set_body_param(last_field_key,
                            last_field_value.substr(string("\r\n").size()),
                            false);
        last_field_key = "";
        last_field_value = "";
        state = BODY_HEADER;
        should_be_empty = false;
    }
    else if (should_be_empty && !line.empty())
        throw Server::Exception("Unsupported file type: " +
                                string("application/octet-stream"));
    else
        last_field_value += "\r\n" + line;
    return state;
}

Request *parse_raw_request(char *raw_headers, size_t length)
{
    Request *req = nullptr;
    string boundary, last_field_key, last_field_value;
    bool should_be_empty;
    try
    {
        State state = REQ;
        vector<string> headers = split(string(raw_headers), "\r\n", false);
        check_data(raw_headers, length);
        size_t real_body_size = calculate_real_body_size(raw_headers);
        for (size_t header_index = 0; header_index < headers.size(); header_index++)
        {
            string line = headers[header_index];
            switch (state)
            {
            case REQ:
            {
                req = handle_req_state(line, state);
            }
            break;
            case HEADER:
            {
                if (line == "")
                {
                    state = handle_header_state(line, req, real_body_size, boundary);
                    break;
                }
                vector<string> R = split(line, ": ", false);
                if (R.size() != 2)
                    throw Server::Exception("Invalid header");
                req->set_header(R[0], R[1], false);
                if (lowercase(R[0]) == lowercase("Content-Length"))
                    if (real_body_size != (size_t)atol(R[1].c_str()))
                        return NULL;
            }
            break;
            case BODY:
            {
                state = handle_body_state(req, line, boundary, should_be_empty, last_field_key, last_field_value);
            }
            break;
            case BODY_HEADER:
            {
                if (line == "")
                {
                    state = BODY_BODY;
                    break;
                }
                handle_body_header_state(line, last_field_key, should_be_empty);
            }
            break;
            case BODY_BODY:
            {
                state = handle_body_body_state(req, line, boundary, should_be_empty, last_field_key, last_field_value);
            }
            break;
            }
        }
    }
    catch (const Server::Exception &)
    {
        throw;
    }
    catch (...)
    {
        throw Server::Exception("Error on parsing request");
    }
    return req;
}

Server::Server(int _port, string _address) : port(_port), address(_address) {
#ifdef _WIN32
  WSADATA wsa_data;
  int initializeResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (initializeResult != 0) {
    throw Exception("Error: WinSock WSAStartup failed: " + string(getSocketError()));
  }
#endif

  notfound_handler = nullptr;

  sc = socket(AF_INET, SOCK_STREAM, 0);
  int sc_option = 1;

#ifdef _WIN32
  setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, (char*)&sc_option, sizeof(sc_option));
#else
  setsockopt(sc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sc_option, sizeof(sc_option));
#endif
  if (!ISVALIDSOCKET(sc))
    throw Exception("Error on opening socket: " + string(get_socket_error()));

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(address.c_str());
  serv_addr.sin_port = htons(port);

  if (::bind(sc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    throw Exception("Error on binding: " + string(get_socket_error()));
  }
}

Server::~Server() {
    if (sc >= 0)
    CLOSESOCKET(sc);
    if (notfound_handler)
        delete notfound_handler;
    for (size_t i = 0; i < routes.size(); ++i)
    delete routes[i];

    #ifdef _WIN32
    WSACleanup();
    #endif
}

Server *Server::get_instance(int _port, std::string _address) {
    if (instance == nullptr)
        instance = new Server(_port, _address);
    return instance;
}

void Server::set_notfound(string path) {
    if (notfound_handler)
        delete notfound_handler;
    notfound_handler = new NotFoundHandler(path);
}

void Server::get(string route, RequestHandler *handler) {
    routes.push_back(new Route(GET, route, handler));
}

void Server::post(string route, RequestHandler *handler) {
    routes.push_back(new Route(POST, route, handler));
}

void Server::run() {
  ::listen(sc, 10);

  struct sockaddr_in cli_addr;
  socklen_t clilen;
  clilen = sizeof(cli_addr);
  SOCKET newsc;

  while (true) {
    newsc = ::accept(sc, (struct sockaddr *)&cli_addr, &clilen);
    if (!ISVALIDSOCKET(newsc))
      throw Exception("Error on accept: " + string(get_socket_error()));
    Response *res = NULL;

    try {
      char* data = new char[BUFFER_SIZE + 1];
      size_t recv_len, recv_total_len = 0;
      Request *req = NULL;
      while (!req) {
        recv_len =
            recv(newsc, data + recv_total_len, BUFFER_SIZE - recv_total_len, 0);
        if (recv_len > 0) {
          recv_total_len += recv_len;
          data[recv_total_len >= 0 ? recv_total_len : 0] = 0;
          req = parse_raw_request(data, recv_total_len);
        } else
          break;
      }
      delete[] data;
      if (!recv_total_len) {
        CLOSESOCKET(newsc);
        continue;
      }
      req->log();

      size_t i = 0;
      for (; i < routes.size(); i++) {
        if (routes[i]->match(req->get_method(), req->get_path())) {
          res = routes[i]->handle(req);
          break;
        }
      }
      if (i == routes.size() && notfound_handler) {
        res = notfound_handler->callback(req);
      }
      delete req;
    } catch (const Exception& exc) {
      delete res;
      res = ServerErrorHandler::callback(exc.get_message());
    }

    int si;
    res->log();
    string res_data = res->show(si);
    delete res;
    int wr = send(newsc, res_data.c_str(), si, 0);
    if (wr != si)
      throw Exception("Send error: " + string(get_socket_error()));
    CLOSESOCKET(newsc);
  }
}

Server::Exception::Exception(const string msg) { message = msg; }

string Server::Exception::get_message() const { return message; }

ShowFile::ShowFile(string _path, string _type) {
  path = _path;
  type = _type;
}

Response *ShowFile::callback(Request *req) {
  Response *res = new Response;
  res->set_header("Content-Type", type);
  res->set_body(read_file(path.c_str()));
  return res;
}

ShowPage::ShowPage(string _path)
    : ShowFile(_path, "text/" + get_extension(_path)) {}

ShowImage::ShowImage(string _path)
    : ShowFile(_path, "image/" + get_extension(_path)) {}


