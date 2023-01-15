#include "../include/user.hpp"

using namespace std;

USER_ID_TYPE User::next_id = 0;

User::User(string name, int fd)
{
    id = ++next_id;
    this->name = name;
    this->fd = fd;
    status = Status::ONLINE;
}

USER_ID_TYPE User::get_id() { return id; }

string User::get_name() { return name; }

int User::get_fd() { return fd; }

void User::set_fd(int fd) { this->fd = fd; }

void User::add_message(Message *message) { messages.push_back(message); }

vector<Message *> User::get_messages()
{
    vector<Message *> return_messages = messages;
    messages.clear();
    return return_messages; 
}

bool User::is_online() { return status == Status::ONLINE; }

void User::set_status(Status status) { this->status = status; }
