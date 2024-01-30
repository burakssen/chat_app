#pragma once

#include <string>

enum class MessageType
{
    MESSAGE,
    MESSAGE_SPLIT,
    COMMAND,
    CONNECT,
    CONNECT_AKN,
    DISCONNECT,
    DISCONNECT_AKN
};

typedef struct Message
{
    MessageType type = MessageType::MESSAGE;
    std::string content;
    std::string username;
    int index = 0;
    int total = 0;
} Message;
