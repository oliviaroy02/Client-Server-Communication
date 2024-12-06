#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")

class SocketClientUDP
{
private:
    SOCKET sock;
    sockaddr_in server;
    WSADATA data;

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

    void createSocket()
    {
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == INVALID_SOCKET)
        {
            std::cout << "Socket creation failed!" << std::endl;
            WSACleanup();
            exit(-1);
        }
    }

    void configureServerAddress(const std::string& ipAddress, int port)
    {
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &server.sin_addr);
    }

    void sendMessage(const std::string& message)
    {
        int sendResult = sendto(sock, message.c_str(), message.size() + 1, 0, (sockaddr*)&server, sizeof(server));
        if (sendResult == SOCKET_ERROR)
        {
            std::cout << "Failed to send message! Error: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            exit(-1);
        }
    }

    void cleanUp()
    {
        closesocket(sock);
        WSACleanup();
    }

public:

    UDPSocketClient()
    {
        initializeWinSock();
        createSocket();
    }

    ~UDPSocketClient()
    {
        cleanUp();
    }

    void runClient(const std::string& ipAddress, int port, const std::string& message)
    {
        configureServerAddress(ipAddress, port);
        sendMessage(message);
    }
};

int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        std::cout << "Please provide a message to send!" << std::endl;
        return -1;
    }

    std::string message(argv[1]);

    UDPSocketClient udpClient;
    udpClient.runClient("127.0.0.1", 54000, message);

    return 0;
}
