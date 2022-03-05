#pragma once

#include <memory>

#include "../game.hpp"
#include "../structs.hpp"
#include "tcpSocket.hpp"
#include "../utils/utils.hpp"

class serverConnection {
 public:
    bool valid = false;
	std::shared_ptr<TCPConnection> connection;

	bool connectionCheck() const;
	
	int recv(char* data, int bytesToRead);
	int send(std::string_view data);

	void join(std::string serverButtonText);
	void join(int serverListIndex);
	void join(std::string_view ip, unsigned int portStart);
	void join(ServerListEntry serverListEntry);
	void join(std::shared_ptr<TCPConnection> con);
	serverConnection() {};
	serverConnection(serverConnection &&) = default;
	serverConnection(const serverConnection &) = default;
	serverConnection &operator=(serverConnection &&) = default;
	serverConnection &operator=(const serverConnection &) = default;
	void close();

 private:
};

inline std::shared_ptr<serverConnection> g_serverConnection;