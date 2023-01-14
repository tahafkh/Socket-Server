#ifndef __SERVER_HPP__
#define __SERVER_HPP__
#include "defs.hpp"
#include "user.hpp"
#include "message.hpp"
#include <vector>
#include <string>

class MessageHandler;

class Server
{
public:
  Server(Server &) = delete;
  void operator=(const Server &) = delete;
  static Server *get_instance(int _port, std::string _address);

  void add_user(std::string username, int fd);
  std::vector<USER_ID_TYPE> get_user_list();
  std::string get_user_name(USER_ID_TYPE id);
  USER_STATUS_TYPE send_message(Message* message, USER_ID_TYPE id);
  std::vector<Message *> get_messages(int fd);
  USER_ID_TYPE get_user_id(int fd);

  void run();

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

  int accept_new_client();
  void disconnect_user(int fd);

  const char* handle_message(char* buffer, int fd);
  const char* handle_connect_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd);
  const char* handle_list_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
  const char* handle_info_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
  const char* handle_send_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd);
  const char* handle_recieve_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd);

  int fd;
  std::string address;
  int port;

  std::vector<User *> users;
  MessageHandler *message_handler;
};
#endif