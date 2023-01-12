#include "client.hpp"
#include "../includes/header.hpp"
#include "../tools/config.hpp"
#include "../tools/utilities.hpp"

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>


using namespace std;

Client::Client(){}

int Client::connect_server(int port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    { // checking for errors
        write(1, "Error in connecting to server\n", 31);
    }

    return fd;
}


void Client::run(int command_channel, int data_channel) 
{
    char received_command[2048] = {0};
    char received_data[4096] = {0};

    int command_fd = connect_server(command_channel);
    write(1, "connected to command channel\n", strlen("connected to command channel\n"));

    int data_fd = connect_server(data_channel);
    write(1, "connected to data channel\n", strlen("connected to data channel\n"));

    while (1) {

        char command[128];
        cout << ">>> ";    
        bzero(command, 128);
        cin.getline (command, 128);
        send(command_fd, command, 128, 0);
        bzero(received_command, sizeof(received_command));
        read(command_fd, received_command, sizeof(received_command));
        bzero(received_data, sizeof(received_data));
        read(data_fd, received_data, sizeof(received_data));
        vector<string> s_command = split(command, ' ');
        cout << "command output: " << received_command << endl;
        cout << "data output: " << received_data << endl;
        // create file

        if(s_command[0] == "retr")
        {
            ofstream outfile;
            outfile.open(s_command[1]);
            outfile << received_data;
            outfile.close();
        }    
    }
}

int main() 
{
    Config config (CONFIG_FILE_PATH);
    Client client;
    int command_channel_port = config.command_channel_port;
    int data_channel_port = config.data_channel_port;
    client.run(command_channel_port, data_channel_port);
    return 0;
}
