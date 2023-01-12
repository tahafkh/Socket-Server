#include "user_handler.hpp"

using namespace std;

UserHandler::UserHandler(Config configparser)
{
    users_info = configparser.get_users_info();
    files = configparser.get_files();
    users = {};
}


void UserHandler::add_user(int command_socket, int data_socket) 
{
    users.push_back(new Connect(command_socket, data_socket));
}

Connect* UserHandler::find_user_with_fd(int socket) {

    for(uint i = 0; i < users.size(); ++i)
        if (users[i]->command_socket == socket)
            return users[i];
    return nullptr;
}

User* UserHandler::find_user_with_username(string username) {
    for(uint i = 0; i < users_info.size(); ++i)
        if (users_info[i]->get_username() == username)
            return users_info[i];
    return nullptr;
}

bool UserHandler::admin_files(string filename) {
    for (uint i = 0; i < files.size(); i++) {
        if (files[i] == filename)
            return true;
    }
    return false;
}

