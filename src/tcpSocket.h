#pragma once

#ifdef _WIN32 
#pragma comment(lib, "Ws2_32")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#endif

#include <vector>
#include <string>



class TcpConnection {
public:
    SOCKET sockfd;
    sockaddr_in address;

    TcpConnection(SOCKET sockfd_in, sockaddr_in address_in);
    TcpConnection(std::string ip, std::string port);
    ~TcpConnection();

    void Close();
    int RecvBlocking(size_t bytesToRead, char* data_out);
    int SendBlocking(size_t bytesToSend, char* data);
    std::string GetAddressString();
};

class TcpSocket {
public:
    int sockfd;
    std::vector<TcpConnection> connections;

    TcpSocket(const unsigned short port, const unsigned int backlog);

	~TcpSocket();
    void Close();
    TcpConnection* Accept();
};

void SocketInitalization();

