#include "Api.h"

Api::Api()
{
}

Api &Api::GetInstance()
{
    static Api instance;
    return instance;
}

void Api::Init()
{
    this->m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "Client socket: " << this->m_clientSocket << std::endl;
    if (this->m_clientSocket == -1)
    {
        throw std::runtime_error("Error creating client socket.");
    }

    if (this->m_serverIp.empty())
    {
        throw std::runtime_error("Server IP not set.");
    }

    this->m_serverAddress.sin_family = AF_INET;
    this->m_serverAddress.sin_port = htons(this->m_PORT);
    inet_pton(AF_INET, this->m_serverIp.c_str(), &(this->m_serverAddress.sin_addr));

    if (connect(this->m_clientSocket, reinterpret_cast<struct sockaddr *>(&this->m_serverAddress), sizeof(this->m_serverAddress)) == -1)
    {
        close(this->m_clientSocket);
        throw std::runtime_error("Error connecting to the server.");
    }
}

void Api::Send(const Message &msg)
{
    Message newMsg;
    std::cout << "Sending message to server: " << msg.content << std::endl;
    newMsg.type = msg.type;
    newMsg.content = msg.content;
    newMsg.username = msg.username;
    std::cout << sizeof(Message) << std::endl;
    size_t a = send(this->m_clientSocket, &newMsg, sizeof(Message), 0);
    std::cout << a << std::endl;
}

void Api::Receive(Message &msg)
{
    memset(&msg, 0, sizeof(Message));
    int bytesReceived = recv(this->m_clientSocket, &msg, sizeof(Message), 0);
    if (bytesReceived <= 0)
    {
        throw std::runtime_error("Server disconnected.");
    }
}

void Api::Close()
{
    close(this->m_clientSocket);
}

void Api::SetServerIp(const std::string &ip)
{
    this->m_serverIp = ip;
}

void Api::SetServerPort(int port)
{
    this->m_PORT = port;
}