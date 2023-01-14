#include "../include/server.hpp"
#include "../include/utilities.hpp"
#include "../include/messagehandler.hpp"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server* Server::instance = nullptr;

static const char *get_socket_error()
{
    return strerror(errno);
}

using namespace std;

Server::Server(int _port, string _address) {
    port = _port;
    address = _address;
    message_handler = new MessageHandler();

    fd = socket(AF_INET, SOCK_STREAM, 0);
    int sc_option = 1;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &sc_option, sizeof(sc_option));

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(address.c_str());
    serv_addr.sin_port = htons(port);

    if (::bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    throw Exception("Error on binding: " + string(get_socket_error()));
    }
}

Server::~Server() {
    if (fd >= 0)
        close(fd);
    delete message_handler;
    for (size_t i = 0; i < users.size(); ++i)
        delete users[i];
}

Server *Server::get_instance(int _port, std::string _address) {
    if (instance == nullptr)
        instance = new Server(_port, _address);
    return instance;
}

Server::Exception::Exception(const string msg) { message = msg; }

string Server::Exception::get_message() const { return message; }

void Server::add_user(string username, int fd) {
    auto user = find_user_by_name(users, username);
    if (user != users.end()) {
        (*user)->set_fd(fd);
        (*user)->set_status(User::Status::ONLINE);
        return;
    }
    users.push_back(new User(username, fd));
}

vector<USER_ID_TYPE> Server::get_user_list() {
    vector<USER_ID_TYPE> user_list;
    for (size_t i = 0; i < users.size(); ++i)
        user_list.push_back(users[i]->get_id());
    return user_list;
}

string Server::get_user_name(USER_ID_TYPE id) {
    auto user = find_user_by_id(users, id);
    if (user == users.end())
        throw Exception("User not found");
    return (*user)->get_name();
}

vector<Message *> Server::get_messages(int fd) {
    auto user = find_user_by_fd(users, fd);
    if (user == users.end())
        throw Exception("User not found");
    return (*user)->get_messages();
}

USER_ID_TYPE Server::get_user_id(int fd) {
    auto user = find_user_by_fd(users, fd);
    if (user == users.end())
        throw Exception("User not found");
    return (*user)->get_id();
}

USER_STATUS_TYPE Server::send_message(Message *message, USER_ID_TYPE id) {
    auto user = find_user_by_id(users, id);
    if (user == users.end())
        throw Exception("User not found");
    const char* content = message_handler->prepare_recievereply_message(message);
    if ((*user)->is_online()) {
        if (write((*user)->get_fd(), content, sizeof(content)) < 0)
            throw Exception("Error on writing to socket: " + string(get_socket_error()));
    }
    else (*user)->add_message(message);
    return (USER_STATUS_TYPE)(*user)->is_online();
}

int Server::accept_new_client()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
        throw Exception("Error on accepting new client: " + string(get_socket_error()));
    return client_fd;
}

void Server::disconnect_user(int fd)
{
    auto user = find_user_by_fd(users, fd);
    if (user == users.end())
        throw Exception("User not found");
    (*user)->set_status(User::Status::OFFLINE);
}

void Server::run()
{
    int max_fd = fd;
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(fd, &master_set);

    while(true)
    {
        try{
            working_set = master_set;
            if (select(max_fd + 1, &working_set, NULL, NULL, NULL) < 0)
                throw Exception("Error on select: " + string(get_socket_error()));
            for (int i = 0; i <= max_fd; ++i) {
                if (FD_ISSET(fd, &working_set)) {
                    if (i == fd) {
                        cout << "New client connected" << endl;
                        int client_fd = accept_new_client();
                        FD_SET(client_fd, &master_set);
                        if (client_fd > max_fd)
                            max_fd = client_fd;
                    }
                    else {
                        char* request = prepare_buffer(BUFFER_SIZE);
                        int bytes_read = read(i, request, BUFFER_SIZE);
                        if (bytes_read < 0) {
                            throw Exception("Error on reading from socket: " + string(get_socket_error()));
                        }
                        else if (bytes_read == 0) {
                            close(i);
                            FD_CLR(i, &master_set);
                            disconnect_user(i);
                        }
                        else {
                            const char* response = handle_message(request, i);
                            if (response != nullptr) {
                                if (write(i, response, sizeof(response)) < 0)
                                    throw Exception("Error on writing to socket: " + string(get_socket_error()));
                                delete[] response;
                            }
                        }
                        delete[] request;
                    }
                }
            }
        } catch (const Exception& e) {
            cout << e.get_message() << endl;
        }
    }
}

const char* Server::handle_message(char* buffer, int fd)
{
    MESSGAE_TYPE_TYPE type;
    MESSAGE_ID_TYPE id;
    MESSAGE_LENGTH_TYPE length;
    int offset = message_handler->get_message_headers(buffer, type, id, length);
    if (type == Message::Type::CONNECT)
    {
        return handle_connect_message(buffer, offset, length, fd);
    }
    else if (type == Message::Type::LIST)
    {
        return handle_list_message(buffer, offset, length);
    }
    else if (type == Message::Type::INFO)
    {
        return handle_info_message(buffer, offset, length);
    }
    else if (type == Message::Type::SEND)
    {
        return handle_send_message(buffer, offset, length, fd);
    }
    else if (type == Message::Type::RECIEVE)
    {
        return handle_recieve_message(buffer, offset, length, fd);
    }
    else
    {
        throw Exception("Unknown message type");
    }
}

const char* Server::handle_connect_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd)
{
    int username_length = length - int(sizeof(MESSAGE_ID_TYPE)) - int(sizeof(MESSGAE_TYPE_TYPE));
    char* username = prepare_buffer(username_length + 1);
    memcpy(username, buffer+offset, username_length);
    username[username_length] = '\0';
    offset += (int)username_length;
    add_user(string(username), fd);
    delete[] username;

    return message_handler->prepare_connack_message();
}

const char* Server::handle_list_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
    vector<USER_ID_TYPE> user_list = get_user_list();
    return message_handler->prepare_listreply_message(user_list);
}

const char* Server::handle_info_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
    USER_ID_TYPE user_id;
    memcpy(&user_id, buffer+offset, sizeof(user_id));
    offset += (int)sizeof(user_id);
    string user_name = get_user_name(user_id);

    return message_handler->prepare_inforeply_message(user_name);
}

const char* Server::handle_send_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd)
{
    USER_ID_TYPE reciever_id;
    memcpy(&reciever_id, buffer+offset, sizeof(reciever_id));
    offset += (int)sizeof(reciever_id);
    int message_length = length - (int)sizeof(MESSAGE_ID_TYPE) - (int)sizeof(MESSGAE_TYPE_TYPE) - (int)sizeof(USER_ID_TYPE);
    char* message = prepare_buffer(message_length + 1);
    memcpy(message, buffer+offset, message_length);
    message[message_length] = '\0';
    offset += message_length;
    USER_ID_TYPE sender_id = get_user_id(fd);
    Message* message_ = new Message(sender_id, string(message));
    delete[] message;
    USER_STATUS_TYPE status = send_message(message_, reciever_id);

    return message_handler->prepare_sendreply_message(status);
}

const char* Server::handle_recieve_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length, int fd)
{
    vector<Message*> messages = get_messages(fd);
    USER_ID_TYPE user_id = get_user_id(fd);
    for (auto message : messages)
    {
        send_message(message, user_id);
        sleep(DELAY);
    }
    return nullptr;
}

int main(int argc, char *argv[])
{
    string info = argv[1], address;
    int port;
    istringstream iss(info);
    getline(iss, address, ':');
    iss >> port;

    Server *server = Server::get_instance(port, address);
    server->run();
    return 0;
}