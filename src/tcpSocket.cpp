/*
Simple wrapper for TCP sockets

@TODO(Assblaster): proper comments and docstrings
@TODO(Assblaster): make calls non blocking
@TODO(Assblaster): some code cleanup and "optimization"

@TODO(Noche): add real c++ exceptions instead of runtime_errors
@TODO(Noche): make this compile on linux (should only need an include and
linking)
*/

#include "tcpSocket.hpp"

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "structs.hpp"  // for WIN_LIN

#define BUFFERSIZE 1024

static constexpr int maxBacklog = 128;
static constexpr int maxReadSize = 4096;

static inline void throwSafe() {
	char error[256];
	
	throw std::runtime_error(std::system_category().message(WIN_LIN(WSAGetLastError(), errno)));
	// strerror_r(errno, error, sizeof(error))
}

TCPConnection::TCPConnection(std::string_view ip, int port) {
	addrinfo serverAddress;
	memset(&serverAddress, 0, sizeof serverAddress);
	serverAddress.ai_family = AF_INET;
	serverAddress.ai_socktype = SOCK_STREAM;

	addrinfo* serverInfo;

	if (getaddrinfo(ip.data(), std::to_string(port).c_str(), &serverAddress, &serverInfo) != 0)
		throw std::runtime_error("getaddrinof failed");

	addrinfo* p;
	for (p = serverInfo; p != NULL; p = p->ai_next) {
		if ((socketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket failed trying next");
			continue;
		}

		if (connect(socketFD, p->ai_addr, p->ai_addrlen) == -1) {
			WIN_LIN(closesocket, ::close)(socketFD);
			perror("connect failed trying next");
			continue;
		}

		break;
	}

	if (p == NULL) throw std::runtime_error("Unable to connect to server");

	freeaddrinfo(serverInfo);

	address = *((sockaddr_in*)p->ai_addr);
}

TCPConnection::~TCPConnection() {
	if (socketFD != -1) close();
}

void TCPConnection::close() {
	if (socketFD == -1) throw std::runtime_error("Connection already closed");

	WIN_LIN(closesocket, ::close)(socketFD);
	socketFD = -1;
}

int TCPConnection::recv(char* data, int bytesToRead) {
	/*
	reads up to bytesToRead bytes from socket
	returns number of bytes read
	*/

	if (socketFD == -1)
		throw std::runtime_error("Trying to recv from closed session");

	int bytesRead = ::recv(socketFD, data, bytesToRead, 0);
	if (bytesRead == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0;
		}
		throwSafe();
	}

	if (bytesRead == 0) {
		close();
	}

	return bytesRead;
}

int TCPConnection::send(std::string_view data) {
	/*
	sends up to bytesToSend bytes to socket
	returns number of bytes sent
	*/
	if (socketFD == -1)
		throw std::runtime_error("Trying to send on closed session");


	if (data.empty()) {
		throw std::invalid_argument("Data is empty");
	}

	auto bytesWritten = ::send(socketFD, data.data(), data.size(), 0);
	if (bytesWritten == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0;
		}
		throwSafe();
	}

	return bytesWritten;
}

std::string_view TCPConnection::getAddressString() {
	char buffer[INET_ADDRSTRLEN];
	char ip[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &address.sin_addr, ip, sizeof ip);

	snprintf(buffer, sizeof buffer, "%s:%d", ip, ntohs(address.sin_port));

	return buffer;
}

TCPSocket::TCPSocket(const unsigned short port) {
	// initalize our socket
	socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socketFD == -1) throw std::runtime_error("Failed to create socket");

	// configure our socket to ignore "port already in use error"
	int yes = 1;
	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes,
	               sizeof(yes)) == -1)
		throw std::runtime_error("Failed to configure socket");

	// make socket non-blocking
	unsigned long iagree = 1; // set just in case
	if (WIN_LIN(ioctlsocket, ioctl)(socketFD, FIONBIO, &iagree) == -1)
		throw std::runtime_error("Failed to configure socket");

	// configure our socket address
	sockaddr_in socketAddress;
	memset(&socketAddress, 0, sizeof socketAddress);
	socketAddress.sin_family = AF_INET;    // ipv4
	socketAddress.sin_port = htons(port);  // port
	socketAddress.sin_addr.s_addr =
	    INADDR_ANY;  // listen on all available interfaces

	// bind our socket to interfaces
	if (bind(socketFD, (sockaddr*)&socketAddress, sizeof socketAddress) == 1)
		throw std::runtime_error("Failed to bind socket");

	// start listening for connections on the socket
	if (listen(socketFD, maxBacklog) == -1)
		throw std::runtime_error("Failed to listen on socket");
}

TCPSocket::~TCPSocket() {
	if (socketFD != -1) close();
}

void TCPSocket::close() {
	if (socketFD == -1) throw std::runtime_error("Socket already closed");

	for (TCPConnection connection : connections) {
		if (connection.socketFD != -1) connection.close();
	}

	WIN_LIN(closesocket, ::close)(socketFD);
	socketFD = -1;
}

TCPConnection* TCPSocket::accept() {
	sockaddr_in address;
	socklen_t addressLenght = sizeof address;

	SOCKET recvfd;
	if ((recvfd = ::accept(socketFD, (sockaddr*)&address, &addressLenght)) == -1)
		throw std::runtime_error("Failed to accept new connection");

	TCPConnection connection = TCPConnection(recvfd, address);
	connections.push_back(connection);

	return &connections.back();
}