#ifndef __IOHANDLER_HPP__
#define __IOHANDLER_HPP__
#include <string>

class Client;

class IOHandler
{
public:
    IOHandler(Client *client);
    ~IOHandler();

    void handle_command(std::string command);
    
    static void show(std::string message);
private:
    void handle_list_command();
    void handle_send_command(std::string username, std::string message);
    void handle_exit_command();

    Client *client;
};
#endif