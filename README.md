# Computer Network - First Computer Assignment

## Mohammad Taha Fakharian & Negar Moradi
## FTP Server

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

```cpp
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

This class contains the following methods
Constructor Server: Sets the data channel and command channel ports and creates the config and command handler classes.

```cpp
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
```

### run :

runs the server, sets the fds and uses select in an infinite loop so that there is no blocking. It receives commands from all clients in a loop and sends them the appropriate response from the data channel and command channel.

```cpp

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

```

### setup_server:

creates a socket in the given port, sets the information related to the struct socket, such as sin_family and sin_add, and finally binds and listens.

```cpp
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

```

### accept_new_client:

Accepts the client.

```cpp

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
```

### client:

Similar to the server class, this class handles the socket part of the system and is the endpoint that actually executes and has an int main function. In the main function, the start method is called. In the start method, first we connect the data channel and command channel to the server using the port number.

```cpp

class Client
{
public:
    Client();
    void run(int command_channel, int data_channel);
    int connect_server(int port);
};

```

Then, the program executes a non-stop for loop to get the commands and send them to the server to fetch the needed data and responses. The implementation details are shown in the code below. Note that for the retr command, after receiving the needed data (which is the binary form of the downloaded file), we have to make a new file and write the fetched data in it.

```cpp

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

```

```cpp

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

```

### command_handler:

This class connects the commands to their respective functions and sets the data and command channel. Methods of this class:

```cpp

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

```

### CommandHandler constructor:

Sets the logger and user-handler.

```cpp
CommandHandler::CommandHandler(UserHandler* user_handler1, Config configparser, Logger* logger) {
    this->user_handler = user_handler1;
    this->logger = logger;
}
```

### run_commands:

First, it parses the command and then connects it to the corresponding method. This method also handles some login errors.

```cpp

vector<string> CommandHandler::run_commands(char* command, int user_socket)
{

    vector<string> command_parts = parse_msg(command);

    Connect* client = user_handler->find_user_with_fd(user_socket);

    if (client == nullptr)
        return {ERROR, SPACE};

    if (command_parts[0] == USER_COMMAND)
        return handle_user(command_parts.size(), command_parts[1], client);

    else if (command_parts[0] == PASS_COMMAND)
        return handle_pass(command_parts.size(), command_parts[1], client);


```

### handle user:

sets the state of the client in "waiting for password" mode. Username sets the client.

```cpp
vector<std::string> CommandHandler::handle_user(int size, string username, Connect* client)
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if(client->get_state() != Connect::State::WAITING_FOR_USERNAME)
        return {BAD_SEQUENCE, SPACE};

    User* user = user_handler->find_user_with_username(username);

    if (user == nullptr)
        return {INVALID_USER_PASS, SPACE};

    client->set_state(Connect::State::WAITING_FOR_PASSWORD);
    client->set_user(user);

```

### handle \_pass:

takes the state of the client to logged in and logs.

```cpp
vector<std::string> CommandHandler::handle_pass(int size, string password, Connect* client)
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if(client->state != Connect::State::WAITING_FOR_PASSWORD)
        return {BAD_SEQUENCE, SPACE};

    if (client->get_user_info()->get_password() != password)
        return {INVALID_USER_PASS, SPACE};

    client->set_state(Connect::State::LOGGED_IN);

```

### handle_retr:

reads the content of the file and sends it to the client. Finally, it reduces the volume of the client.

```cpp
std::vector<std::string> CommandHandler::handle_retr(int size, string file_name, Connect* client)
{

    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};

    if (!check_file_name(file_name))
        return {SYNTAX_ERROR, SPACE};

```

### handle_upload:

It finds the given file and then copies it to the file folder

```cpp
std::vector<std::string> CommandHandler::handle_upload(int size, string file_name, Connect* client)
{
    if (size != 2)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};

    if (!user_has_access_to_file(file_name, client))
        return {FILE_UNAVAILABLE, SPACE};

    string file_path = client->get_directory() + file_name;

```

### handle_help:

returns help commands.

```cpp
std::vector<std::string> CommandHandler::handle_help(int size, Connect* client)
{
    if (size != 1)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};

    string info = INFO;
    info = info + USER_DESCRIPTION + PASS_DESCRIPTION +  RETR_DESCRIPTION + UPLOAD_DESCRIPTION +
            HELP_DESCRIPTION + QUIT_DESCRIPTION;

```

### handle_quit:

Change the client state to "waiting for username".

```cpp
vector<string> CommandHandler::handle_quit(int size, Connect* client)
{
    if (size != 1)
        return {SYNTAX_ERROR, SPACE};

    if (check_user_logged_in(client))
        return {NEED_ACCOUNT, SPACE};

    if (client->state == Connect::State::WAITING_FOR_PASSWORD)
        return {ERROR, SPACE};

    if(client->state == Connect::State::WAITING_FOR_USERNAME)
        return {ERROR, SPACE};

```

### config class:

This class handles the config.json file.

```cpp
class Config
{
public:
    Config(std::string path);
    int data_channel_port;
    int command_channel_port;
    void parser(std::string path);
    std::vector<User*> get_users_info();
    std::vector<std::string> get_files();

private:
    std::vector<User*> users_info;
    std::vector<std::string> files;
};
```

Methods of this class:

### parser:

extracts the contents of the config.json file and pours it into the vector.

```cpp
void Config::parser(string path)
{
    ifstream file(path);
    nlohmann::json json;
    file >> json;

    command_channel_port = json["commandChannelPort"];
    data_channel_port = json["dataChannelPort"];

    for (auto& user: json["users"])
    {
        string name = user["user"].get<string>();
        string password = user["password"].get<string>();
        bool admin = user["admin"].get<string>() == "true";
        int size = stoi(user["size"].get<string>());
        users_info.push_back(new User(name, password, admin, size));
    }
    for (auto& file: json["files"])
        files.push_back(file.get<string>());
}

```

The rest of the methods of this class output file information.

### connect class:

It keeps information related to connections such as username, status, data channel ports, command and current directory. The methods of this class output the information it holds.

```cpp
class Connect
{
public:

    enum State
    {
        WAITING_FOR_USERNAME,
        WAITING_FOR_PASSWORD,
        LOGGED_IN,
    };

    Connect(int command_socket, int data_socket);
    State get_state();
    std::string get_username();
    std::string get_directory();
    User* get_user_info();
    void set_state(State _state);
    void set_user(User* _user);
    void set_dir(std::string path);
    bool can_download(double file_size);
    bool can_upload(double file_size);
    bool check_admin();
    State state;
    int command_socket;
    int data_socket;

private:

    std::string directory;
    User* user;
};

```

### logger class:

Logs requested in classes are taken with this log class. The date is also logged.

```cpp
class Logger
{
public:
    Logger(std::string path_);
    void write_message(std::string message);

private:
    std::string path;
    std::fstream log_stream;
};
```

### user_handler class:

This class keeps the information of users, users and files. Methods of this class:

```cpp
class UserHandler
{
public:
    UserHandler(Config configparser);
    std::vector<std::string> files;
    void add_user(int command_socket, int data_socket);
    Connect* find_user_with_fd(int socket);
    User* find_user_with_username(std::string username);
    bool admin_files(std::string file_path);
    int check;

private:
    std::vector<User*> users_info;
    std::vector<Connect*> users;

};

```

add_user: Adds a new connection to the users it maintains.

```cpp
void UserHandler::add_user(int command_socket, int data_socket)
{
    users.push_back(new Connect(command_socket, data_socket));
}

```

find_user_with_socket: returns the corresponding user by getting fd.

```cpp
Connect* UserHandler::find_user_with_fd(int socket) {

    for(uint i = 0; i < users.size(); ++i)
        if (users[i]->command_socket == socket)
            return users[i];
    return nullptr;
}

```

find_user_info_with_username: Finds and returns the user by username.

```cpp
User* UserHandler::find_user_with_username(string username) {
    for(uint i = 0; i < users_info.size(); ++i)
        if (users_info[i]->get_username() == username)
            return users_info[i];
    return nullptr;
}
```

admin_fles: Checks whether the user can access the given file or not

```cpp

bool UserHandler::admin_files(string filename) {
    for (uint i = 0; i < files.size(); i++) {
        if (files[i] == filename)
            return true;
    }
    return false;
}
‍‍‍
```

### user class:

This class keeps user information such as username, password, admin, and size, and using its methods, you can get this information, find out about the correctness of the password, and reduce the size of the user.

```cpp
class User
{
public:
    User(std::string username_, std::string password_, bool admin_, int size_);
    void change_size(int size_);
    std::string get_username();
    std::string get_password();
    bool is_admin();
    bool is_correct(std::string username_, std::string password_);
    int get_size();
private:
    std::string username;
    std::string password;
    bool admin;
    int size;
};
```

## Outputs:

check username
<Image title='username' alt="Alt text" src="./pics/ftp-server/username.png">

check password and login

<Image title='login' alt="Alt text" src="./pics/ftp-server/login.png">

invalid password

<Image title='pass' alt="Alt text" src="./pics/ftp-server/invalidPass.png">

bad Sequence for command

<Image title='badSeq' alt="Alt text" src="./pics/ftp-server/badSequence.png">

invalid username pass

<Image title='invalid' alt="Alt text" src="./pics/ftp-server/invalidUsernamePass.png">

need account

<Image title='needAccount' alt="Alt text" src="./pics/ftp-server/needAccount.png">

retr

<Image title='retr' alt="Alt text" src="./pics/ftp-server/retr.png">

upload

<Image title='upload' alt="Alt text" src="./pics/ftp-server/upload.png">

upload file

<Image title='uploadFile' alt="Alt text" src="./pics/ftp-server/uploadFile.png">

help

<Image title='help' alt="Alt text" src="./pics/ftp-server/help.png">

syntax error

<Image title='syntaxError' alt="Alt text" src="./pics/ftp-server/syntaxError.png">

quit

<Image title='quit' alt="Alt text" src="./pics/ftp-server/quit.png">

retr for normal user

<Image title='retrNotAdmin' alt="Alt text" src="./pics/ftp-server/retrNotAdmin.png">

file unavailable for not accessing

<Image title='fileUnavailable' alt="Alt text" src="./pics/ftp-server/fileUnavailable.png">
