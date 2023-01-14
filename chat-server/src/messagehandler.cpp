#include "../include/messagehandler.hpp"
#include "../include/utilities.hpp"
#include "../include/message.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>

using namespace std;

MessageHandler::MessageHandler()
{
};

int MessageHandler::get_message_headers(char* buffer, MESSGAE_TYPE_TYPE& type, MESSAGE_ID_TYPE& id, MESSAGE_LENGTH_TYPE& length)
{
    int offset = 0;
    memcpy(&type, buffer+offset, sizeof(type));
    offset += (int)sizeof(type);
    memcpy(&id, buffer+offset, sizeof(id));
    offset += (int)sizeof(id);
    memcpy(&length, buffer+offset, sizeof(length));
    offset += sizeof(MESSAGE_LENGTH_TYPE);
    return offset;
}

int MessageHandler::set_message_headers(char* buffer, MESSGAE_TYPE_TYPE type, MESSAGE_ID_TYPE id)
{
    int offset = 0;
    memcpy(buffer+offset, &type, sizeof(type));
    offset += (int)sizeof(type);
    memcpy(buffer+offset, &id, sizeof(id));
    offset += (int)sizeof(id);
    return offset;
}

const char* MessageHandler::prepare_connack_message()
{
    char* reply = new char[BUFFER_SIZE];
    bzero(reply, BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::CONNACK, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = sizeof(MESSAGE_ID_TYPE) + sizeof(MESSGAE_TYPE_TYPE);
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    return reply;    
}

const char* MessageHandler::prepare_listreply_message(const vector<USER_ID_TYPE>& user_list)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::LISTREPLY, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) +
                                     (int)sizeof(USER_ID_TYPE) * user_list.size();
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    for (auto user_id : user_list)
    {
        memcpy(reply+reply_offset, &user_id, sizeof(user_id));
        reply_offset += (int)sizeof(user_id);
    }
    return reply;
}

const char* MessageHandler::prepare_inforeply_message(const string& user_name)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::INFOREPLY, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) 
                                        + user_name.length();
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, user_name.c_str(), user_name.length());
    return reply;
}

const char* MessageHandler::prepare_sendreply_message(USER_STATUS_TYPE status)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::SENDREPLY, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) 
                                        + (int)sizeof(USER_STATUS_TYPE);
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, &status, sizeof(status));
    return reply;
}

const char* MessageHandler::prepare_recievereply_message(Message* message)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    string content = message->get_content();
    USER_ID_TYPE sender_id = message->get_sender_id();
    reply_offset += set_message_headers(reply, Message::Type::RECIEVEREPLY, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) 
                                        + (int)sizeof(USER_ID_TYPE) + content.length();
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, &sender_id, sizeof(sender_id));
    reply_offset += (int)sizeof(message->get_sender_id());
    memcpy(reply+reply_offset, content.c_str(), content.length());
    return reply;
}

const char* MessageHandler::prepare_connect_message(const std::string& user_name)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::CONNECT, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) + user_name.length();
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, user_name.c_str(), user_name.length());
    return reply;
}

const char* MessageHandler::prepare_list_message()
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::LIST, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE);
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    return reply;
}

const char* MessageHandler::prepare_info_message(USER_ID_TYPE user_id)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::INFO, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) + (int)sizeof(USER_ID_TYPE);
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, &user_id, sizeof(user_id));
    return reply;
}

const char* MessageHandler::prepare_send_message(USER_ID_TYPE user_id, const std::string& content)
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::SEND, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE) + (int)sizeof(USER_ID_TYPE)
                                         + content.length();
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    reply_offset += (int)sizeof(reply_length);
    memcpy(reply+reply_offset, &user_id, sizeof(user_id));
    reply_offset += (int)sizeof(user_id);
    memcpy(reply+reply_offset, content.c_str(), content.length());
    return reply;
}

const char* MessageHandler::prepare_recieve_message()
{
    char* reply = prepare_buffer(BUFFER_SIZE);
    int reply_offset = 0;
    reply_offset += set_message_headers(reply, Message::Type::RECIEVE, Message::get_next_id());
    MESSAGE_LENGTH_TYPE reply_length = (int)sizeof(MESSAGE_ID_TYPE) + (int)sizeof(MESSGAE_TYPE_TYPE);
    memcpy(reply+reply_offset, &reply_length, sizeof(reply_length));
    return reply;
}

