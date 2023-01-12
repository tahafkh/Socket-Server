#ifndef COMMANDHANDLER_HPP_
#define COMMANDHANDLER_HPP_

#include "config.hpp"
#include "logger.hpp"
#include "connect.hpp"
#include "user_handler.hpp"

class CommandHandler 
{
public:
    CommandHandler(UserHandler* user_handler1, Config config, Logger* logger);

    std::vector<std::string> run_commands(char* command, int user_socket);

    std::vector<std::string> handle_user(int size, std::string username, Connect* client);
    std::vector<std::string> handle_pass(int size, std::string password, Connect* client);
    std::vector<std::string> handle_retr(int size, std::string file_name, Connect* client);
    std::vector<std::string> handle_upload(int size, std::string file_name, Connect* client);
    std::vector<std::string> handle_help(int size, Connect* client);
    std::vector<std::string> handle_quit(int size, Connect* client);

    bool check_file_name(std::string file_name);
    bool user_has_access_to_file(std::string file_name, Connect* client);
    std::vector<std::string> command_parts;
    UserHandler* user_handler;

private:
    Logger* logger;
    bool check_user_logged_in(Connect *client);
};

#endif
