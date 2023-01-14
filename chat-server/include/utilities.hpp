#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__
#include "defs.hpp"
#include <vector>
#include <string>

class User;

char* prepare_buffer(int length);

std::vector<std::string> split(std::string str, char delimiter);

std::vector<User*>::const_iterator find_user_by_id(const std::vector<User*>& users, USER_ID_TYPE id);
std::vector<User*>::const_iterator find_user_by_fd(const std::vector<User*>& users, int fd);
std::vector<User*>::const_iterator find_user_by_name(const std::vector<User*>& users, std::string name);
#endif