#ifndef __INCLUDE_USER_HPP__
#define __INCLUDE_USER_HPP__
#include "defs.hpp"
#include "message.hpp"
#include <vector>
#include <string>

class User
{
public:
    User(std::string name, int fd);

    USER_ID_TYPE get_id();
    std::string get_name();
    int get_fd();

    void set_fd(int fd);

    void add_message(Message *message);
    std::vector<Message *> get_messages();

    enum Status : USER_STATUS_TYPE
    {
        ONLINE,
        OFFLINE,
    };

    bool is_online();
    void set_status(Status status);

private:
    static USER_ID_TYPE next_id;

    USER_ID_TYPE id;
    std::string name;
    int fd;
    Status status;
    std::vector<Message *> messages;
};

#endif