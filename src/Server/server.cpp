#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Common/Message.h"

constexpr int PORT = 8080;
constexpr int MAX_CLIENTS = 10;

std::vector<int> clients;

void broadcast(const Message &msg, int sender)
{
    std::cout << "Broadcasting message from client " << sender << ": " << msg.content << std::endl;
    for (int client : clients)
    {
        if (client != sender)
        {
            send(client, &msg, sizeof(Message), 0);
        }
    }
}

void handleClient(int clientSocket)
{
    Message msg;
    while (true)
    {
        std::cout << "Waiting for message from client " << clientSocket << std::endl;
        size_t size = sizeof(Message);
        int bytesReceived = recv(clientSocket, &msg, sizeof(Message), 0);

        if (msg.type == MessageType::CONNECT)
        {
            msg.type = MessageType::CONNECT_AKN;
            send(clientSocket, &msg, sizeof(Message), 0);
        }

        if (msg.type == MessageType::DISCONNECT)
        {
            msg.type = MessageType::DISCONNECT_AKN;
            send(clientSocket, &msg, sizeof(Message), 0);
        }

        if (bytesReceived <= 0)
        {
            // Handle disconnection
            std::cout << "Client " << clientSocket << " disconnected." << std::endl;
            close(clientSocket);

            // Remove the client from the vector
            clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
            break;
        }

        // Broadcast the message to all clients
        broadcast(msg, clientSocket);
    }
}

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating server socket." << std::endl;
        return -1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error binding server socket." << std::endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, MAX_CLIENTS) == -1)
    {
        std::cerr << "Error listening for connections." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true)
    {
        sockaddr_in clientAddress{};
        socklen_t clientAddressSize = sizeof(clientAddress);

        int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressSize);
        if (clientSocket == -1)
        {
            std::cerr << "Error accepting client connection." << std::endl;
            continue;
        }

        // Add the client to the vector
        clients.push_back(clientSocket);

        std::cout << "Client " << clientSocket << " connected." << std::endl;

        // Start a new thread to handle the client
        std::thread(handleClient, clientSocket).detach();
    }

    close(serverSocket);

    return 0;
}
