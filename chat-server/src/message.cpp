#include "../include/message.hpp"

using namespace std;

MESSAGE_ID_TYPE Message::next_id = 0;

Message::Message(USER_ID_TYPE sender_id, string content)
{
    this->sender_id = sender_id;
    this->content = content;
}

MESSAGE_ID_TYPE Message::get_next_id() { return next_id++; }

USER_ID_TYPE Message::get_sender_id() { return sender_id; }

string Message::get_content() { return content; }

