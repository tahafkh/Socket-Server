
#include "logger.hpp"

using namespace std;

Logger::Logger(string path_)    
{
    path = path_;
    log_stream.open(path, std::fstream::app);

}

void Logger::write_message(string message) 
{
    auto now = std::chrono::system_clock::now();
    std::time_t cur_time = std::chrono::system_clock::to_time_t(now);
    log_stream << std::ctime(&cur_time);
    log_stream << message << endl;
}
