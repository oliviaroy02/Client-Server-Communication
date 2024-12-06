#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib, "ws2_32.lib")

class ClientTCP
{
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    const string serverIP;
    const int serverPort;

public:
    ClientTCP(const string& ipAddress, int port) 
        : serverIP(ipAddress), serverPort(port), clientSocket(INVALID_SOCKET) 
    {
        WORD version = MAKEWORD(2, 2);
        WSADATA data;
        int wsResult = WSAStartup(version, &data);
        if (wsResult != 0)
        {
            throw runtime_error("WinSock could not be started! Error #" + to_string(wsResult));
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET)
        {
            WSACleanup();
            throw runtime_error("Socket could not be created! Error #" + to_string(WSAGetLastError()));
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    }

    ~TcpClient() 
    {
        closesocket(clientSocket);
        WSACleanup();
    }

    void connectToServer()
    {
        int Connection = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (connection == SOCKET_ERROR)
        {
            closesocket(clientSocket);
            WSACleanup();
            throw runtime_error("Cannot connect to server! Error #" + to_string(WSAGetLastError()));
        }
    }

    void sendMessage(const string& message)
    {
        int sendResult = send(clientSocket, message.c_str(), message.size() + 1, 0);
        if (sendResult == SOCKET_ERROR)
        {
            throw runtime_error("Failed to send message! Error #" + to_string(WSAGetLastError()));
        }
    }

    string receiveMessage()
    {
        char buffer[4096];
        ZeroMemory(buffer, 4096);

        int bytesReceived = recv(clientSocket, buffer, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            throw runtime_error("Error receiving message! Error #" + to_string(WSAGetLastError()));
        }
        else if (bytesReceived == 0)
        {
            return "";
        }

        return string(buffer, 0, bytesReceived);
    }
};

int main()
{
    const string IPaddress = "127.0.0.1"; 
    int portNumber = 54000;

    try 
    {
        ClientTCP client(IPaddress, portNumber);
        client.connectToServer();

        string userInput;

        do
        {
            std::cout << "Enter message: ";
            getline(cin, userInput);

            if (!userInput.empty()) 
            {
                client.sendMessage(userInput);

                string serverResponse = client.receiveMessage();
                if (!serverResponse.empty()) 
                {
                    cout << "Server response: " << serverResponse << endl;
                }
            }

        } while (!userInput.empty());
    }
    catch (const exception& ex) 
    {
        cerr << "Error: " << ex.what() << endl;
    }

    return 0;
}
