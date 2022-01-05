/*
Simple wrapper for TCP sockets

@TODO(Assblaster): proper comments and docstrings
@TODO(Assblaster): make calls non blocking
@TODO(Assblaster): some code cleanup and "optimization"

@TODO(Noche): add real c++ exceptions instead of runtime_errors
@TODO(Noche): make this compile on linux (should only need an include and linking)
*/

#include "tcpSocket.h"

#include <stdio.h>
#include <iostream>
#include "structs.hpp"	// for WIN_LIN

#define BUFFERSIZE 1024

TcpConnection::TcpConnection(SOCKET sockfd_in, sockaddr_in address_in) {
	sockfd = sockfd_in;
	address = address_in;
}

TcpConnection::TcpConnection(std::string ip, std::string port) {
	addrinfo serverAddress;
	memset(&serverAddress, 0, sizeof serverAddress);
	serverAddress.ai_family = AF_INET;
	serverAddress.ai_socktype = SOCK_STREAM;

	addrinfo *serverInfo;
	
	if (getaddrinfo(ip.c_str(), port.c_str(), &serverAddress, &serverInfo) != 0)
		throw std::runtime_error("getaddrinof failed");
	
	addrinfo* p;
	for (p = serverInfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket failed trying next");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			WIN_LIN(closesocket, close)(sockfd);
			perror("connect failed trying next");
			continue;
		}
		
		break;
	}

	if (p == NULL)
		throw std::runtime_error("Unable to connect to server");

	freeaddrinfo(serverInfo);

	address = *((sockaddr_in*)p->ai_addr);
}

TcpConnection::~TcpConnection() {
	if (sockfd != -1)
		Close();
}

void TcpConnection::Close() {
	if (sockfd == -1)
		throw std::runtime_error("Connection already closed");

	WIN_LIN(closesocket, close)(sockfd);
	sockfd = -1;
}

int TcpConnection::RecvBlocking(size_t bytesToRead, char* data_out) {
	/*
	reads up to bytesToRead bytes from socket
	returns number of bytes read
	*/

	if (sockfd == -1)
		throw std::runtime_error("Trying to recv from closed session");

	return recv(sockfd, data_out, (int)bytesToRead, 0);
}

int TcpConnection::SendBlocking(size_t bytesToSend, char* data) {
	/*
	sends up to bytesToSend bytes to socket
	returns number of bytes sent
	*/
	if (sockfd == -1)
		throw std::runtime_error("Trying to send on closed session");

	return send(sockfd, data, (int)bytesToSend, 0);
}

std::string TcpConnection::GetAddressString() {
	char buffer[INET_ADDRSTRLEN];
	char ip[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, & address.sin_addr, ip, sizeof ip);

	snprintf(buffer, sizeof buffer, "%s:%d", ip, ntohs(address.sin_port));

	return buffer;
}


TcpSocket::TcpSocket(const unsigned short port, const unsigned int backlog) {
	// initalize our socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1)
		throw std::runtime_error("Failed to create socket");

	// configure our socket to ignore "port already in use error"
	int yes = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof yes) == -1)
		throw std::runtime_error("Failed to configure socket");

	// configure our socket address
	sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof socketAddress);
	socketAddress.sin_family = AF_INET;          // ipv4
	socketAddress.sin_port = htons(port);        // port
	socketAddress.sin_addr.s_addr = INADDR_ANY;  // listen on all available interfaces

	// bind our socket to interfaces
	if (bind(sockfd, (sockaddr*)&socketAddress, sizeof socketAddress) == 1)
		throw std::runtime_error("Failed to bind socket");

	// start listening for connections on the socket
	if (listen(sockfd, backlog) == -1)
		throw std::runtime_error("Failed to listen on socket");
}

TcpSocket::~TcpSocket() {
	if (sockfd != -1)
		Close();
}

void TcpSocket::Close() {
	if (sockfd == -1)
		throw std::runtime_error("Socket already closed");

	for (TcpConnection connection : connections) {
		if (connection.sockfd != -1)
			connection.Close();
	}

	WIN_LIN(closesocket, close)(sockfd);
	sockfd = -1;
}

TcpConnection* TcpSocket::Accept() {
        sockaddr_in address;
        socklen_t addressLenght = sizeof address;

        SOCKET recvfd;
        if ((recvfd = accept(sockfd, (sockaddr*)&address, &addressLenght)) == -1)
            throw std::runtime_error("Failed to accept new connection");

        TcpConnection connection = TcpConnection(recvfd, address);
        connections.push_back(connection);

        return &connections.back();
    }

void SocketInitalization() {
	// Only required to be called before socket use for windows build
#ifdef _WIN32
    WSAData wsaData;

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        throw std::runtime_error("Failed to intalize WSA");
    }
#endif
}

