#include "command_handler.hpp"
#include "../includes/header.hpp"
#include "utilities.hpp"
#include "user.hpp"
#include "connect.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

using namespace std;

CommandHandler::CommandHandler(UserHandler* user_handler1, Config configparser, Logger* logger) {
    this->user_handler = user_handler1;
    this->logger = logger;
}

std::vector<std::string> parse_msg(char* msg){
    std::string str;
    std::vector<std::string> parsed;
    for(uint i = 0; i < strlen(msg); i++){
        if(msg[i] == ' '){
        parsed.push_back(str);
        str = "";
        continue;
    }
    str += msg[i];
  }
  if(str != "")
    parsed.push_back(str);
  return parsed;
}

string read_file_to_string(const string &path)
{
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}



string fix_address(string current_path, string type)
{
    string temp = type + current_path + " > file.txt";
    if(system(temp.c_str()) != 0)
        return ERROR;
    string final_path = read_file_to_string("file.txt");
    final_path.pop_back();
    if(system("rm file.txt") != 0)
        return ERROR;
    return final_path;
}

bool CommandHandler::check_user_logged_in(Connect *client)
{
    if (client->get_state() != Connect::State::LOGGED_IN) 
        return true;
    return false;
}

vector<string> CommandHandler::run_commands(char* command, int user_socket) 
{

    vector<string> command_parts = parse_msg(command);

    Connect* client = user_handler->find_user_with_fd(user_socket);

    if (client == nullptr)
        return {ERROR, SPACE};

    if (command_parts[0] == USER_COMMAND) 
        return handle_user(command_parts.size(), command_parts[1], client);

    else if (command_parts[0] == PASS_COMMAND) 
        return handle_pass(command_parts.size(), command_parts[1], client);

    else if (command_parts[0] == RETR_COMMAND) 
        return handle_retr(command_parts.size(), command_parts[1], client);

    else if (command_parts[0] == UPLOAD_COMMAND)
        return handle_upload(command_parts.size(), command_parts[1], client);

    else if (command_parts[0] == HELP_COMMAND) 
        return handle_help(command_parts.size(), client);


    else if (command_parts[0] == QUIT_COMMAND) 
        return handle_quit(command_parts.size(), client);

    else
        return {SYNTAX_ERROR, SPACE};
}

bool CommandHandler::check_file_name(string file_name) 
{
    if (file_name.find(SLASH) != std::string::npos)
        return false;
    return true;
}

bool CommandHandler::user_has_access_to_file(string file_name, Connect* client) 
{
    if (!user_handler->admin_files(file_name))
        return true;
    else if (client->check_admin())
        return true;
    return false;
}

vector<std::string> CommandHandler::handle_user(int size, string username, Connect* client) 
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if(client->get_state() != Connect::State::WAITING_FOR_USERNAME)
        return {BAD_SEQUENCE, SPACE};

    User* user = user_handler->find_user_with_username(username);

    if (user == nullptr)
        return {INVALID_USER_PASS, SPACE};
    
    client->set_state(Connect::State::WAITING_FOR_PASSWORD);
    client->set_user(user);
    client->set_dir(EMPTY);

    logger->write_message(username + " is trying to login");
    return {USERNAME_ACCEPTED, SPACE};
}

vector<std::string> CommandHandler::handle_pass(int size, string password, Connect* client) 
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if(client->state != Connect::State::WAITING_FOR_PASSWORD)
        return {BAD_SEQUENCE, SPACE};

    if (client->get_user_info()->get_password() != password)
        return {INVALID_USER_PASS, SPACE};

    client->set_state(Connect::State::LOGGED_IN);
    logger->write_message(client->get_username() + COLON + "logged in.");
    return {SUCCESSFUL_LOGIN, SPACE};
}

std::vector<std::string> CommandHandler::handle_retr(int size, string file_name, Connect* client) 
{

    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};
        
    if (!check_file_name(file_name))
        return {SYNTAX_ERROR, SPACE};
        
    if (!user_has_access_to_file(file_name, client))
        return {FILE_UNAVAILABLE, SPACE};

    string file_path = client->get_directory() + file_name;
    string size_command = "stat -c%s " + file_path + " > " + "size.txt";
    if (system(size_command.c_str()) != 0)
        return {ERROR, SPACE};
    
    std::ifstream myfile("size.txt", std::ios::in| std::ios::binary);
    double file_size;
    myfile >> file_size;
    if (system("rm size.txt") != 0)
        return {ERROR, SPACE};
    if (client -> can_download(file_size) == false)
        return {DOWNLOAD_LIMIT_SIZE, SPACE};

    string temp = "cp " + file_path + " file.txt";
    if (system(temp.c_str()) != 0)
        return {ERROR, SPACE};
    string final_message = read_file_to_string("file.txt");
    if (system("rm file.txt") != 0)
        return {ERROR, SPACE};
        
    client->get_user_info()->change_size(file_size);
    logger->write_message(client->get_username() +  COLON + file_name + " downloaded.");
    return {SUCCESSFUL_DOWNLOAD, final_message};
}

std::vector<std::string> CommandHandler::handle_upload(int size, string file_name, Connect* client) 
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};
        
    if (!user_has_access_to_file(file_name, client))
        return {FILE_UNAVAILABLE, SPACE};

    string file_path = client->get_directory() + file_name;
    string size_command = "stat -c%s " + file_path + " > " + "size.txt";
    if (system(size_command.c_str()) != 0)
        return {ERROR, SPACE};
    
    std::ifstream myfile("size.txt", std::ios::in| std::ios::binary);
    double file_size;
    myfile >> file_size;
    if (system("rm size.txt") != 0)
        return {ERROR, SPACE};
    if (client -> can_upload(file_size) == false)
        return {DOWNLOAD_LIMIT_SIZE, SPACE};
        
    struct stat sb;
    char* dir = "Files"; 
    if (stat(dir, &sb) != 0) 
    {
        string make_folder = "mkdir " + client->get_directory() + "Files";
        if (system(make_folder.c_str()) != 0) 
            return {ERROR, SPACE};
    }

    string input_file_name = split(file_name, '/').back();
    string temp = "cp " + file_path + " ./Files/" + file_name;
    if (system(temp.c_str()) != 0)
        return {ERROR, SPACE};

    string final_message = read_file_to_string("./Files/" + file_name);
        
    client->get_user_info()->change_size(file_size);
    logger->write_message(client->get_username() +  COLON + file_name + " downloaded.");
    return {SUCCESSFUL_DOWNLOAD, final_message};
}

std::vector<std::string> CommandHandler::handle_help(int size, Connect* client) 
{
    if (size != 1)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};
    
    string info = INFO;
    info = info + USER_DESCRIPTION + PASS_DESCRIPTION +  RETR_DESCRIPTION + UPLOAD_DESCRIPTION +
            HELP_DESCRIPTION + QUIT_DESCRIPTION;

    logger->write_message(client->get_username() + " see help message");
    return {info, SPACE};
}

vector<string> CommandHandler::handle_quit(int size, Connect* client) 
{
    if (size != 1)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};
        
    if (client->state == Connect::State::WAITING_FOR_PASSWORD)
        return {ERROR, SPACE};

    if(client->state == Connect::State::WAITING_FOR_USERNAME)
        return {ERROR, SPACE};

    client->set_state(Connect::State::WAITING_FOR_USERNAME);
    logger->write_message(client->get_username() + COLON + "logged out.");
    return {SUCCESSFUL_QUIT, SPACE};
}
