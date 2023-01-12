#ifndef SERVER_H_
#define SERVER_H_

#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "../tools/config.hpp"
#include "../tools/command_handler.hpp"
#include "../tools/logger.hpp"


class Server 
{
public:
    Server(Server &) = delete;
    void operator=(const Server&) = delete;
    static Server* get_instance(int command_channel, int data_channel ,Config configparser);
    void run();
    int setup_server(int port);
    int accept_new_client(int server_fd);

private:
    Server(int command_channel, int data_channel ,Config configparser);
    ~Server();
    
    static Server* instance;
    CommandHandler* command_handler;
    Logger* logger;
    int command_channel_port;
    int data_channel_port;
};

#endif
