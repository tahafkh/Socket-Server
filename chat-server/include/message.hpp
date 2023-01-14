#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__
#include "defs.hpp"
#include <string>

class Message
{
public:
    Message(USER_ID_TYPE sender_id, std::string content);

    static MESSAGE_ID_TYPE get_next_id();

    USER_ID_TYPE get_sender_id();
    std::string get_content();

    enum Type : MESSGAE_TYPE_TYPE
    {
        CONNECT = 1,
        CONNACK = 2,
        LIST = 3,
        LISTREPLY = 4,
        INFO = 5,
        INFOREPLY = 6,
        SEND = 7,
        SENDREPLY = 8,
        RECIEVE = 9,
        RECIEVEREPLY = 10,
    };

private:
    static MESSAGE_ID_TYPE next_id;

    USER_ID_TYPE sender_id;
    std::string content;
};
#endif