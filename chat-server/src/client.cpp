#include "../include/client.hpp"
#include "../include/defs.hpp"
#include "../include/utilities.hpp"
#include "../include/iohandler.hpp"
#include "../include/messagehandler.hpp"
#include "../include/message.hpp"
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

static const char *get_socket_error()
{
    return strerror(errno);
}

using namespace std;

void Client::connect_to_server()
{;
    struct sockaddr_in serv_addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(address.c_str());

    if (::connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        throw Exception("Error on connecting: " + string(get_socket_error()));
    }
    int reply_offset = 0;
    const char* request = message_handler->prepare_connect_message(username, reply_offset);
    if (write(fd, request, reply_offset) < 0) {
        throw Exception("Error on writing: " + string(get_socket_error()));
    }
    delete[] request;
    char* response = prepare_buffer(BUFFER_SIZE);
    if (read(this->fd, response, BUFFER_SIZE) < 0) {
        throw Exception("Error on reading: " + string(get_socket_error()));
    }
    handle_message(response);
    delete[] response;
}

void Client::recieve_pending_messages()
{
    int reply_offset = 0;
    const char* request = message_handler->prepare_recieve_message(reply_offset);
    if (write(fd, request, reply_offset) < 0) {
        throw Exception("Error on writing: " + string(get_socket_error()));
    }
    delete[] request;
}

Client::Client(int _port, string _address, string _username)
{
    this->port = _port;
    this->address = _address;
    this->username = _username;
    this->io_handler = new IOHandler(this);
    this->message_handler = new MessageHandler();

    try{
        connect_to_server();
    } catch (Exception &e) {
        io_handler->show(e.get_message());
        exit(0);
    }
    recieve_pending_messages();
}

Client::~Client()
{
    delete this->io_handler;
    delete this->message_handler;
}

Client::Exception::Exception(const string message)
{
    this->message = message;
}

string Client::Exception::get_message() const
{
    return this->message;
}

void Client::set_user_id_list(const vector<USER_ID_TYPE> &user_id_list)
{
    this->user_id_list = user_id_list;
}

void Client::update_user_map(const string &user_name)
{
    this->id_to_name[this->pending_user_id] = user_name;
    this->name_to_id[user_name] = this->pending_user_id;
    this->user_name_list.push_back(user_name);
}

void Client::get_username(USER_ID_TYPE user_id)
{
    this->pending_user_id = user_id;
    int reply_offset = 0;
    const char* request = message_handler->prepare_info_message(user_id, reply_offset);
    if (write(this->fd, request, reply_offset) < 0) {
        throw Exception("Error on writing: " + string(get_socket_error()));
    }
    delete[] request;
    char* response = prepare_buffer(BUFFER_SIZE);
    if (read(this->fd, response, BUFFER_SIZE) < 0) {
        throw Exception("Error on reading: " + string(get_socket_error()));
    }
    handle_message(response);
    delete[] response;
}

void Client::get_message(USER_ID_TYPE user_id, const string message)
{
    if (!id_to_name.count(user_id))
        get_username(user_id);
    io_handler->show("<< " + id_to_name[user_id] + ": " + message);
}

vector<string> Client::perform_list()
{
    id_to_name.clear();
    name_to_id.clear();
    user_name_list.clear();
    user_id_list.clear();

    int reply_offset = 0;
    const char* request = message_handler->prepare_list_message(reply_offset);
    if (write(this->fd, request, reply_offset) < 0) {
        throw Exception("Error on writing: " + string(get_socket_error()));
    }
    delete[] request;

    char* response = prepare_buffer(BUFFER_SIZE);
    if (read(this->fd, response, BUFFER_SIZE) < 0) {
        throw Exception("Error on reading: " + string(get_socket_error()));
    }
    handle_message(response);
    delete[] response;

    for (auto user_id : user_id_list) {
        get_username(user_id);
    }
    return user_name_list;
}

void Client::perform_send(string user_name, string message)
{
    if (!name_to_id.count(user_name)) {
        throw Exception("User not found");
    }
    USER_ID_TYPE user_id = name_to_id[user_name];
    int reply_offset = 0;
    const char* request = message_handler->prepare_send_message(user_id, message, reply_offset);
    if (write(this->fd, request, reply_offset) < 0) {
        throw Exception("Error on writing: " + string(get_socket_error()));
    }
    delete[] request;
    char* response = prepare_buffer(BUFFER_SIZE);
    if (read(this->fd, response, BUFFER_SIZE) < 0) {
        throw Exception("Error on reading: " + string(get_socket_error()));
    }
    handle_message(response);
    delete[] response;
}

void Client::perform_exit()
{
    exit(0);
}

void Client::run()
{
    int max_fd = fd;
    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(fd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    while (true) {
        try{
            working_set = master_set;
            if (select(max_fd + 1, &working_set, NULL, NULL, NULL) < 0) {
                throw Exception("Error on select: " + string(get_socket_error()));
            }
            for(int fd_ = 0; fd_ <= max_fd; fd_++) {
                if (FD_ISSET(fd_, &working_set)) {
                    if (fd_ == fd){
                        char* response = prepare_buffer(BUFFER_SIZE);
                        if (read(fd, response, BUFFER_SIZE) < 0) {
                            throw Exception("Error on reading: " + string(get_socket_error()));
                        }
                        handle_message(response);
                        delete[] response;
                    }
                    else {
                        char* buffer = prepare_buffer(BUFFER_SIZE);
                        if (read(STDIN_FILENO, buffer, BUFFER_SIZE) < 0) {
                            throw Exception("Error on reading: " + string(get_socket_error()));
                        }
                        string command = string(buffer);
                        delete[] buffer;
                        io_handler->handle_command(command.erase(command.length() - 1));
                    }
                }
            }
        } catch (const Exception& e) {
            IOHandler::show(e.get_message());
        }
    }
}

void Client::handle_message(char* buffer)
{
    MESSGAE_TYPE_TYPE type;
    MESSAGE_ID_TYPE id;
    MESSAGE_LENGTH_TYPE length;
    int offset = message_handler->get_message_headers(buffer, type, id, length);
    if (type == Message::Type::CONNACK)
    {
        return handle_connack_message(buffer, offset, length);
    }
    else if (type == Message::Type::LISTREPLY)
    {
        return handle_listreply_message(buffer, offset, length);
    }
    else if (type == Message::Type::INFOREPLY)
    {
        return handle_inforeply_message(buffer, offset, length);
    }
    else if (type == Message::Type::SENDREPLY)
    {
        return handle_sendreply_message(buffer, offset, length);
    }
    else if (type == Message::Type::RECIEVEREPLY)
    {
        return handle_recievereply_message(buffer, offset, length);
    }
    else
    {
        throw Client::Exception("Unknown message type");
    }
}

void Client::handle_connack_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
}

void Client::handle_listreply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
    vector<USER_ID_TYPE> user_list;
    int num = ((int)length - (int)sizeof(MESSAGE_ID_TYPE) - (int)sizeof(MESSGAE_TYPE_TYPE)) / (int)sizeof(USER_ID_TYPE);
    for (int i = 0; i < num; i++)
    {
        USER_ID_TYPE user_id;
        memcpy(&user_id, buffer+offset, sizeof(user_id));
        offset += (int)sizeof(user_id);
        user_list.push_back(user_id);
    }
    set_user_id_list(user_list);
}

void Client::handle_inforeply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
    int num = ((int)length - (int)sizeof(MESSAGE_ID_TYPE) - (int)sizeof(MESSGAE_TYPE_TYPE));
    char *user_name = prepare_buffer(num + 1);
    memcpy(user_name, buffer+offset, num);
    user_name[num] = '\0';
    update_user_map(string(user_name));
}

void Client::handle_sendreply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
}

void Client::handle_recievereply_message(char* buffer, int offset, MESSAGE_LENGTH_TYPE length)
{
    USER_ID_TYPE sender_id;
    memcpy(&sender_id, buffer+offset, sizeof(sender_id));
    offset += (int)sizeof(sender_id);
    if (sender_id != NO_SENDER_ID)
    {
        int num = ((int)length - (int)sizeof(MESSAGE_ID_TYPE) - (int)sizeof(MESSGAE_TYPE_TYPE) - (int)sizeof(USER_ID_TYPE));
        char *message = prepare_buffer(num + 1);
        memcpy(message, buffer+offset, num);
        message[num] = '\0';
        get_message(sender_id, string(message));
    }
}

int main(int argc, char* argv[])
{
    string info = argv[1], address, username = argv[2];
    int port;
    istringstream iss(info);
    getline(iss, address, ':');
    iss >> port;

    Client client(port, address, username);
    client.run();
    return 0;
}

