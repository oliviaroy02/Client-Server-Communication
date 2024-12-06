#include "pch.h"
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

class ServerTCP
{
    void bindSocket(SOCKET socketListening, const char* ipAddress, int port)
    {
        sockaddr_in hint;
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        hint.sin_addr.S_un.S_addr = inet_addr(ipAddress);

        int bindResult = bind(socketListening, (sockaddr*)&hint, sizeof(hint));
        if (bindResult == SOCKET_ERROR)
        {
            std::cout << "Socket bind failed!" << std::endl;
            closesocket(socketListening);
            WSACleanup();
            exit(1);
        }
    }

    void listenOnSocket(SOCKET socketListening)
    {
        int listenResult = listen(socketListening, SOMAXCONN);
        if (listenResult == SOCKET_ERROR)
        {
            std::cout << "Socket listen failed!" << std::endl;
            closesocket(socketListening);
            WSACleanup();
            exit(1);
        }
    }

    void printClientInfo(sockaddr_in client)
    {
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        ZeroMemory(host, NI_MAXHOST);
        ZeroMemory(service, NI_MAXSERV);

        int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0);
        if (result == 0)
        {
            std::cout << host << " connected on port " << service << std::endl;
        }
        else
        {
            inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
            std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;
        }
    }

public:

    void startListening(const char* ipAddress, int port)
    {

        WSADATA data;
        WORD version = MAKEWORD(2, 2);
        int wsResult = WSAStartup(version, &data);
        if (wsResult != 0)
        {
            std::cout << "WinSock initialization failed!" << std::endl;
            exit(1);
        }


        SOCKET socketListening = socket(AF_INET, SOCK_STREAM, 0);
        if (socketListening == INVALID_SOCKET)
        {
            std::cout << "Socket creation failed!" << std::endl;
            WSACleanup();
            exit(1);
        }

        bindSocket(socketListening, ipAddress, port);

        listenOnSocket(socketListening);

        std::cout << "Server is listening on " << ipAddress << ":" << port << std::endl;

        sockaddr_in client;
        int clientSize = sizeof(client);
        SOCKET clientSocket = accept(socketListening, (sockaddr*)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cout << "Client connection failed!" << std::endl;
            closesocket(socketListening);
            WSACleanup();
            exit(1);
        }

        printClientInfo(client);

        closesocket(socketListening);

        char message[4096];
        while (true)
        {
            ZeroMemory(message, 4096);
            int bytesReceived = recv(clientSocket, message, 4096, 0);
            if (bytesReceived == SOCKET_ERROR)
            {
                std::cout << "ERROR in recv()" << std::endl;
                break;
            }
            if (bytesReceived == 0)
            {
                std::cout << "Client disconnected!" << std::endl;
                break;
            }

            std::cout << "Received: " << std::string(message, 0, bytesReceived) << std::endl;
            send(clientSocket, message, bytesReceived + 1, 0);
        }

        closesocket(clientSocket);
        WSACleanup();
    }
};

int main()
{
    ServerTCP server;
    server.startListening("127.0.0.1", 54000);
    return 0;
}
