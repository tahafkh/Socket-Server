#include "../include/iohandler.hpp"
#include "../include/client.hpp"
#include "../include/utilities.hpp"
#include "../include/defs.hpp"
#include <iostream>

using namespace std;

IOHandler::IOHandler(Client* client)
{
    this->client = client;
}

IOHandler::~IOHandler()
{
}

void IOHandler::handle_command(string command)
{
    vector<string> command_parts = split(command, ' ');
    if (command_parts[1] == LIST_COMMAND)
    {
        handle_list_command();
    }
    else if (command_parts[1] == SEND_COMMAND)
    {
        handle_send_command(command_parts[2], command_parts[3]);
    }
    else if (command_parts[1] == EXIT_COMMAND)
    {
        handle_exit_command();
    }
    else
    {
        throw Client::Exception("Invalid command");
    }
}

void IOHandler::handle_list_command()
{
    vector<string> user_list = client->perform_list();
    for (string user : user_list)
    {
        show("\t -" + user);
    }
}

void IOHandler::handle_send_command(string username, string message)
{
    client->perform_send(username, message);
}

void IOHandler::handle_exit_command()
{
    client->perform_exit();
}

void IOHandler::show(string message)
{
    cout << message << endl;
}
