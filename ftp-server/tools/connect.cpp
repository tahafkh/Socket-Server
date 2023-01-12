#include "connect.hpp"

using namespace std;

Connect::Connect(int command_socket, int data_socket)
{
    this->command_socket = command_socket;
    this->data_socket = data_socket;
    state = WAITING_FOR_USERNAME;
    directory = EMPTY;
    user = nullptr;
}

Connect::State Connect::get_state() {return state;}

string Connect::get_username() {return user->get_username();}

string Connect::get_directory() {return directory;}

User* Connect::get_user_info() {return user;}

void Connect::set_state(Connect::State _state) {state = _state;}

void Connect::set_user(User* _user) {user = _user;}

void Connect::set_dir(string path) {directory = path;}

bool Connect::can_download(double file_size) 
{
    if (user->get_size() >= file_size/1000)
        return true;
    return false;
}

bool Connect::can_upload(double file_size)
{
    if (file_size > 1024)
        return false;
    return true;
}


bool Connect::check_admin() { return user->is_admin();}