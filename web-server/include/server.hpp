#ifndef __SERVER_HPP__
#define __SERVER_HPP__
#include "../include/defs.hpp"
#include "../include/request.hpp"
#include "../include/response.hpp"
#include "../include/route.hpp"
#include <exception>
#include <string>
#include <vector>

#ifdef _WIN32
typedef unsigned SOCKET;
#else
typedef int SOCKET;
#endif

class RequestHandler {
public:
  virtual ~RequestHandler();
  virtual Response *callback(Request *req) = 0;
};

class ShowFile : public RequestHandler {
public:
  ShowFile(std::string _path, std::string _type);
  Response *callback(Request *req);

private:
  std::string path;
  std::string type;
};

class ShowPage : public ShowFile {
public:
  ShowPage(std::string _path);
};

class ShowImage : public ShowFile {
public:
  ShowImage(std::string _path);
};

class ShowDocument : public ShowFile {
public:
  ShowDocument(std::string _path);
};

class ShowAudio : public ShowFile {
public:
  ShowAudio(std::string _path);
};

class Server {
public:
  Server(Server &) = delete;
  void operator=(const Server &) = delete;
  static Server *get_instance(int _port, std::string _address);

  void set_notfound(std::string);

  void run();
  void get(std::string path, RequestHandler *handler);
  void post(std::string path, RequestHandler *handler);


  class Exception : public std::exception {
  public:
    Exception() {}
    Exception(const std::string);

    std::string get_message() const;

  private:
    std::string message;
  };

private:
  Server(int _port, std::string _address);
  ~Server(); 
  static Server *instance;

  SOCKET sc;
  std::string address;
  int port;
  std::vector<Route *> routes;
  RequestHandler *notfound_handler;
};

#endif
