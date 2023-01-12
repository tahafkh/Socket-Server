#ifndef CLIENT_HPP_
#define CLIENT_HPP_


class Client 
{
public:
    Client();
    void run(int command_channel, int data_channel);
    int connect_server(int port);
};

#endif
