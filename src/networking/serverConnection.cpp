#include "serverConnection.hpp"

#include "packetHandler.hpp"
#include "../events.hpp"

bool serverConnection::connectionCheck() const {
	return connection.get() != nullptr;
};

int serverConnection::recv(char *data, int bytesToRead) {
	if (!connectionCheck()) throw std::runtime_error("Connection doesn't exist.");

	return connection->recv(data, bytesToRead);
};

int serverConnection::send(std::string_view data) {
	if (!connectionCheck()) throw std::runtime_error("Connection doesn't exist.");

	return connection->send(data);
};

void serverConnection::join(std::string serverButtonText) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");

	try {
		if (!serverButtonText.starts_with("Server ")) return;
		serverButtonText = serverButtonText.substr(7, 3);

		join(std::stoi(serverButtonText, nullptr, 0));
	} catch (const std::exception &e) {
		spdlog::error("Error while trying to process server button, {}", e.what());
	}
};
void serverConnection::join(int serverListIndex) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");
	try {
		join(g_game->serverListEntries[serverListIndex]);
	} catch (const std::exception &e) {
		spdlog::error("Error while trying to process server button, {}", e.what());
	}
};
void serverConnection::join(std::string_view ip, unsigned int portStart) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");

	int customPortStart = portStart;
	int customPort = customPortStart;

	try {
		connection = std::make_shared<TCPConnection>(ip, customPort);
	} catch (const std::exception) {
		throw std::runtime_error("Custom connection failed to the server.");
	};
	
	g_eventHandler->triggerEventHandshake();
	valid = true;
}
void serverConnection::join(ServerListEntry serverListEntry) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");

	try {
		if (serverListEntry.versionMajor < 36) return;

		std::string ipName(spdlog::fmt_lib::format(
		    "{}.{}.{}.{}", serverListEntry.ip[3], serverListEntry.ip[2],
		    serverListEntry.ip[1], serverListEntry.ip[0]));

		spdlog::info("Connecting to {}:{}", ipName, serverListEntry.port + 1);

		join(ipName, serverListEntry.port + 1);
	} catch (const std::exception &e) {
		spdlog::error("Error while trying to process server button, {}", e.what());
	}
};
void serverConnection::join(std::shared_ptr<TCPConnection> con) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");
	valid = true;
	connection = con;
};

void serverConnection::close() {
	if (connectionCheck() && connection->socketFD != -1) connection->close();
	spdlog::info("Custom connection closed.");
}