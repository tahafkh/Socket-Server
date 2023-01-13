#include "server.hpp"
#include "../tools/config.hpp"
#include "../tools/logger.hpp"
#include "../includes/header.hpp"
#include "../tools/command_handler.hpp"
#include <sys/socket.h>

using namespace std;

int Server::setup_server(int port)
{
    struct sockaddr_in address;
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        printf("error in server socket");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)))
    {
        printf("binding error \n");
        return -1;
    }

    listen(server_fd, 4);

    return server_fd;
}

int Server::accept_new_client(int server_fd)
{
    int client_fd;

    struct sockaddr_in client_address;

    int address_len = sizeof(client_address);

    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);

    if (client_fd < 0)
    {
        printf("Error on accept new client \n");
        exit(1);
    }

    return client_fd;
}

Server::Server(int command_channel, int data_channel, Config configparser)
{
    command_channel_port = command_channel;
    data_channel_port = data_channel;
    logger = new Logger(LOG_FILE);
    command_handler = new CommandHandler(new UserHandler(configparser), configparser, logger);
}

Server::~Server()
{
    delete command_handler;
    delete logger;
}

Server *Server::get_instance(int command_channel, int data_channel, Config configparser)
{
    if (instance == nullptr)
        instance = new Server(command_channel, data_channel, configparser);
    return instance;
}

void Server::run()
{
    int command_server_fd = setup_server(command_channel_port);
    int data_server_fd = setup_server(data_channel_port);

    if (command_server_fd == -1 || data_server_fd == -1)
        return;

    int max_fd = command_server_fd;

    fd_set master_set, working_set;

    FD_ZERO(&master_set);

    FD_SET(command_server_fd, &master_set);

    write(1, "Server is running\n", 18);

    while (1)
    {

        working_set = master_set;

        if (select(max_fd + 1, &working_set, NULL, NULL, NULL) < 0)
        {
            printf("select error \n");
            exit(1);
        }

        for (int fd = 0; fd <= max_fd; ++fd)
        {

            if (FD_ISSET(fd, &working_set))
            {
                // New client
                if (fd == command_server_fd)
                {
                    int new_command_socket = accept_new_client(command_server_fd);

                    int new_data_socket = accept_new_client(data_server_fd);
                    command_handler->user_handler->add_user(new_command_socket, new_data_socket);

                    FD_SET(new_command_socket, &master_set);

                    if (new_command_socket > max_fd)
                        max_fd = new_command_socket;

                    printf("New client connected. fd = %d\n", new_command_socket);
                }

                // Client sending msg
                else
                {
                    char temp_buffer[1024];
                    bzero(temp_buffer, 1024);
                    int bytes_received = read(fd, temp_buffer, sizeof(temp_buffer));
                    if (bytes_received < 0)
                        if (errno != EWOULDBLOCK)
                        {
                            printf("error receiving message from client");
                            close(fd);
                        }

                    if (bytes_received == 0)
                    {
                        printf("client fd = %d closed\n", fd); // EOF

                        close(fd);
                        FD_CLR(fd, &master_set);
                        continue;
                    }
                    // Data is received.
                    if (bytes_received > 0)
                    {
                        vector<string> output = command_handler->run_commands(temp_buffer, fd);
                        send(fd, output[0].c_str(), output[0].size(), 0);
                        send(command_handler->user_handler->find_user_with_fd(fd)->data_socket,
                             output[1].c_str(), output[1].size(), 0);
                    }
                }
            }
        }
    }
}

Server *Server::instance = nullptr;

int main()
{
    Config configparser(CONFIG_FILE_PATH);
    int command_channel_port = configparser.command_channel_port;
    int data_channel_port = configparser.data_channel_port;
    Server *server = Server::get_instance(command_channel_port, data_channel_port, configparser);
    server->run();
    return 0;
}
