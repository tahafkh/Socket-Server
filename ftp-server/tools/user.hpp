#ifndef USER_HPP_
#define USER_HPP_

#include <iostream>
#include <string>

class User
{
public:
    User(std::string username_, std::string password_, bool admin_, int size_);
    void change_size(int size_);
    std::string get_username();
    std::string get_password();
    bool is_admin();
    bool is_correct(std::string username_, std::string password_);
    int get_size();
private:
    std::string username;
    std::string password;
    bool admin;
    int size;
};

#endif
