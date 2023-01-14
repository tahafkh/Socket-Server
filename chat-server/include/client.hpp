#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__
#include "defs.hpp"
#include <vector>
#include <map>
#include <string>

class IOHandler;
class MessageHandler;

class Client
{
public:
    Client(int _port, std::string _address, std::string _username);
    ~Client();

    void set_user_id_list(const std::vector<USER_ID_TYPE>& user_id_list);
    void update_user_map(const std::string& user_name);
    void get_message(USER_ID_TYPE user_id, const std::string message);

    std::vector<std::string> perform_list();
    void perform_send(std::string username, std::string message);
    void perform_exit();

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
    void connect_to_server();
    void get_username(USER_ID_TYPE user_id);

    void handle_message(char* buffer);
    void handle_connack_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
    void handle_listreply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
    void handle_inforeply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
    void handle_sendreply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);
    void handle_recievereply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length);

    int fd;
    std::string address;
    int port;
    std::string username;

    IOHandler *io_handler;
    MessageHandler *message_handler;

    USER_ID_TYPE pending_user_id;
    std::vector<USER_ID_TYPE> user_id_list;
    std::vector<std::string> user_name_list;
    std::map<USER_ID_TYPE, std::string> id_to_name;
    std::map<std::string, USER_ID_TYPE> name_to_id;
};
#endif