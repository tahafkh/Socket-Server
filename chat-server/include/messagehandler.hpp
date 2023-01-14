#ifndef __MESSAGEHANDLER_HPP__
#define __MESSAGEHANDLER_HPP__
#include "defs.hpp"
#include <string>
#include <vector>

class Message;

class MessageHandler
{
public:
    MessageHandler();

    int get_message_headers(char* buffer, MESSGAE_TYPE_TYPE& type, MESSAGE_ID_TYPE& id, MESSAGE_LENGTH_TYPE& length);
    int set_message_headers(char* buffer, MESSGAE_TYPE_TYPE type, MESSAGE_ID_TYPE id);

    const char* prepare_connect_message(std::string username);
    const char* prepare_list_message();
    const char* prepare_info_message(USER_ID_TYPE id);
    const char* prepare_send_message(USER_ID_TYPE id, std::string message);
    const char* prepare_recieve_message();

    const char* prepare_connack_message();
    const char* prepare_listreply_message(std::vector<USER_ID_TYPE> user_list);
    const char* prepare_inforeply_message(std::string username);
    const char* prepare_sendreply_message(USER_STATUS_TYPE status);
    const char* prepare_recievereply_message(Message* message);
private:
};
#endif 