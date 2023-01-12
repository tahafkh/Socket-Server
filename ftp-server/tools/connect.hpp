#ifndef CONNECT_Hpp_
#define CONNECT_Hpp_

#include "user.hpp"
#include "../includes/header.hpp"

class Connect 
{
public:

    enum State 
    {
        WAITING_FOR_USERNAME,
        WAITING_FOR_PASSWORD,
        LOGGED_IN,
    };

    Connect(int command_socket, int data_socket);
    State get_state();
    std::string get_username();
    std::string get_directory();
    User* get_user_info();
    void set_state(State _state);
    void set_user(User* _user);
    void set_dir(std::string path);
    bool can_download(double file_size);
    bool can_upload(double file_size);
    bool check_admin();
    State state; 
    int command_socket;
    int data_socket;

private:

    std::string directory;
    User* user;
};

#endif
