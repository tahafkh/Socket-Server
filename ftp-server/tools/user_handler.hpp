#ifndef USERHANDLER_HPP_
#define USERHANDLER_HPP_

#include "config.hpp"
#include "user.hpp"
#include "connect.hpp"
#include "../includes/header.hpp"

class UserHandler
{
public:
    UserHandler(Config configparser);
    std::vector<std::string> files;
    void add_user(int command_socket, int data_socket);
    Connect* find_user_with_fd(int socket);
    User* find_user_with_username(std::string username);
    bool admin_files(std::string file_path);
    int check;

private:
    std::vector<User*> users_info;
    std::vector<Connect*> users;
    
};

#endif
