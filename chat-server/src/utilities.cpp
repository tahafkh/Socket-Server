#include "../include/utilities.hpp"
#include "../include/user.hpp"
#include <algorithm>
#include <cstring>
#include <sstream>

using namespace std;

char* prepare_buffer(int length)
{
    char* buffer = new char[length];
    bzero(buffer, length);
    return buffer;
}

vector<User*>::const_iterator find_user_by_id(const vector<User*>& users, USER_ID_TYPE id)
{
    return find_if(users.begin(), users.end(), [&id](User* user) { return user->get_id() == id; });
}

vector<User*>::const_iterator find_user_by_fd(const vector<User*>& users, int fd)
{
    return find_if(users.begin(), users.end(), [&fd](User* user) { return user->get_fd() == fd; });
}

vector<User*>::const_iterator find_user_by_name(const vector<User*>& users, string name)
{
    return find_if(users.begin(), users.end(), [&name](User* user) { return user->get_name() == name; });
}

vector<string> split(string str, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream token_stream(str);
    while (getline(token_stream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}
