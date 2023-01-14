#ifndef __DEFS_HPP__
#define __DEFS_HPP__
#include <string>
#include <stdint.h>

#define BUFFER_SIZE 4145152

#define MESSGAE_TYPE_TYPE uint8_t
#define MESSAGE_ID_TYPE uint8_t
#define MESSAGE_LENGTH_TYPE uint8_t

#define USER_ID_TYPE uint16_t
#define USER_STATUS_TYPE uint8_t

#define DELAY 5

const std::string LIST_COMMAND = "list";
const std::string SEND_COMMAND = "send";
const std::string EXIT_COMMAND = "exit";


#endif