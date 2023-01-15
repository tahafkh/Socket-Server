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

    const char* prepare_connect_message(std::string username, int& reply_offset);
    const char* prepare_list_message(int& reply_offset);
    const char* prepare_info_message(USER_ID_TYPE id, int& reply_offset);
    const char* prepare_send_message(USER_ID_TYPE id, std::string message, int& reply_offset);
    const char* prepare_recieve_message(int& reply_offset);

    const char* prepare_connack_message(int& reply_offset);
    const char* prepare_listreply_message(std::vector<USER_ID_TYPE> user_list, int& reply_offset);
    const char* prepare_inforeply_message(std::string username, int& reply_offset);
    const char* prepare_sendreply_message(USER_STATUS_TYPE status, int& reply_offset);
    const char* prepare_recievereply_message(Message* message, int& reply_offset);
private:
};
#endif 