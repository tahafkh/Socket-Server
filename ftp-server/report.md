# FTP Server

## Taha Ebrahim Zade & Negar Moradi

### Overview:

In this project, we have attempted to design an FTP server using socket programming in the C++ programming language. The software architecture is an Object-Oriented paradigm with a focus on SOLID design principles.

### Overall Design:

The overall architecture consists of these directories:

- /build
- /client
- /includes
- /server
- /tools
- Makefile

1. build: this directory -whose name is defined in the Makefile- is the directory in which the build files -i.o files with .o format- are stored.

2. client: client folder is related to client classes and functions.

3. include: the directory that hoses the header files.

4. server: server folder is related to server classes and functions.

5. tools: tools folder is related to the rest of the classes used in the project, including command_handler, config and connect.

### server class

This class is responsible for the socket part of the system. Server has a run method that is called in the main function. In the said method, we first setup two sockets channels for command and data. These sockets are later passed to every individual User instance. These User objects represent a Client.out process. The setup_channel method is responsible for making a socket, binding it, and calls listen on the socket.

```
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

```
