#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime> 

class Logger 
{
public:
    Logger(std::string path_);
    void write_message(std::string message);

private:
    std::string path;
    std::fstream log_stream;
};

#endif
