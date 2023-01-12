#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <string>
#include <vector>

#include "user.hpp"

class Config 
{
public:
    Config(std::string path);
    int data_channel_port;
    int command_channel_port;
    void parser(std::string path);
    std::vector<User*> get_users_info();
    std::vector<std::string> get_files();

private:
    std::vector<User*> users_info;
    std::vector<std::string> files;
};

#endif
