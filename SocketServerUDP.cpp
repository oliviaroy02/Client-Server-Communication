#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")

class UDPServer
{
private:
    SOCKET sock;
    sockaddr_in serverHint, client;
    WSADATA data;
    int clientLength;
    char message[1024];

    void initializeWinSock()
    {
        WORD version = MAKEWORD(2, 2);
        int wsResult = WSAStartup(version, &data);
        if (wsResult != 0)
        {
            std::cout << "WinSock initialization failed!" << std::endl;
            exit(-1);
        }
    }

    void createAndBindSocket(int port)
    {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == INVALID_SOCKET)
        {
            std::cout << "Socket creation failed!" << std::endl;
            WSACleanup();
            exit(-1);
        }

        serverHint.sin_family = AF_INET;
        serverHint.sin_port = htons(port);
        serverHint.sin_addr.S_un.S_addr = ADDR_ANY;

        int bindResult = bind(sock, (sockaddr*)&serverHint, sizeof(serverHint));
        if (bindResult == SOCKET_ERROR)
        {
            std::cout << "Socket bind failed!" << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(-1);
        }
    }

    void handleClientCommunication()
    {
        clientLength = sizeof(client);
        ZeroMemory(message, 1024);

        int bytesIn = recvfrom(sock, message, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR)
        {
            std::cout << "Error receiving data from client: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(-1);
        }

        char clientIP[256];
        ZeroMemory(clientIP, 256);
        inet_ntop(AF_INET, &client.sin_addr, clientIP, 256);

        std::cout << "Message received from " << clientIP << " : " << message << std::endl;
    }

    void cleanUp()
    {
        closesocket(sock);
        WSACleanup();
    }

public:

    UDPServer()
    {
        initializeWinSock();
    }

    ~UDPServer()
    {
        cleanUp();
    }

    void runServer(int port)
    {
        createAndBindSocket(port);
        while (true)
        {
            handleClientCommunication();
        }
    }
};

int main()
{
    UDPServer udpServer;
    udpServer.runServer(54000);
    return 0;
}
