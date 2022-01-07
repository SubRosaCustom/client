#pragma once

#ifdef _WIN32
#pragma comment(lib, "Ws2_32")

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int SOCKET;
#endif

#include <string>
#include <string_view>
#include <vector>

class TCPConnection {
 public:
	SOCKET socketFD;
	sockaddr_in address;

	TCPConnection(SOCKET sockfd_in, sockaddr_in address_in)
	    : socketFD(sockfd_in), address(address_in){};
	TCPConnection(std::string_view ip, int port);
	~TCPConnection();

	void close();
	int recv(char* data, int bytesToRead);
	int send(std::string_view data);
	std::string_view getAddressString();
};

class TCPSocket {
 public:
	int socketFD;
	std::vector<TCPConnection> connections;

	TCPSocket(const unsigned short port);

	~TCPSocket();
	void close();
	TCPConnection* accept();
};
