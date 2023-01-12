#include "config.hpp"
#include "../includes/njson.hpp"

#include <iostream>
#include <fstream>


using namespace std;

Config::Config(string path) 
{
    parser(path);
}

void Config::parser(string path)
{
    ifstream file(path);
    nlohmann::json json;
    file >> json;

    command_channel_port = json["commandChannelPort"];
    data_channel_port = json["dataChannelPort"];

    for (auto& user: json["users"])
    {
        string name = user["user"].get<string>();
        string password = user["password"].get<string>();
        bool admin = user["admin"].get<string>() == "true";
        int size = stoi(user["size"].get<string>());
        users_info.push_back(new User(name, password, admin, size));
    }
    for (auto& file: json["files"])
        files.push_back(file.get<string>());
}

std::vector<User*> Config::get_users_info() 
{
    return users_info;
}

std::vector<std::string> Config::get_files() 
{
    return files;
}
