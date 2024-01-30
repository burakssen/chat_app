#pragma once

#include <iostream>
#include <cstring>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Common/Message.h"

class Api
{
    Api();

public:
    static Api &GetInstance();
    void Init();
    void Send(const Message &msg);
    void Receive(Message &msg);
    void Close();

    void SetServerIp(const std::string &ip);
    void SetServerPort(int port);

private:
    int m_clientSocket = -1;
    std::string m_serverIp;
    int m_PORT = 8080;
    sockaddr_in m_serverAddress{};
    Message m_message;
};