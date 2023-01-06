#include "../include/server.hpp"
#include "../include/utilities.hpp"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>


#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 //win xp
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
//POSIX sockets
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

static const char* get_socket_error() {
#ifdef _WIN32
    static char message[256];
    message[0] = '\0';
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, WSAGetLastError(), 0, (LPSTR)&message, sizeof(message), NULL);
    char *newline = strrchr(message, '\n');
    if (newline) *newline = '\0';
    return message;
#else
    return strerror(errno);
#endif
}

using namespace std;

class NotFoundHandler : public ShowPage {
public:
    NotFoundHandler(string _path) : ShowPage(_path) {}
    Response *callback(Request *req);
};

Response *NotFoundHandler::callback(Request *req) {
    Response *res = ShowPage::callback(req);
    res->set_code(404);
    return res;
}

class ServerErrorHandler {
public:
    static Response *callback(string msg);
};

Response *ServerErrorHandler::callback(string msg) {
    Response *res = new Response(500);
    res->set_header("Content-Type", "application/json");
    res->set_body("{ \"code\": \"500\", \"message\": \"" + msg + "\" }\n");
    return res;
}

void split(string str, string separator, int max, vector<string> &results) {
  int i = 0;
  size_t found = str.find_first_of(separator);

  while (found != string::npos) {
    if (found > 0)
      results.push_back(str.substr(0, found));
    str = str.substr(found + 1);
    found = str.find_first_of(separator);

    if (max > -1 && ++i == max)
      break;
  }

  if (str.length() > 0) results.push_back(str);
}
