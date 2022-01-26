#include "serverConnection.hpp"

bool serverConnection::connectionCheck() const {
	return connection.get() != nullptr;
};

bool serverConnection::handshake() const {
	if (!connectionCheck()) throw std::runtime_error("Connection doesn't exist.");

	if (connection->send(fmt::format("{},{}{}{}", subRosaCustomMagic,
	                                 customVersion[0], customVersion[1],
	                                 customVersion[2])) <= 0)
		throw std::runtime_error("Handshake send failed.");

	char data[46];
	std::memset(&data, 0, sizeof(data));
	if (auto amountRead = connection->recv((char *)&data, sizeof(data))) {
		std::string str(data, amountRead);
        g_utils->log(INFO, fmt::format("hnad: {}", str));
		if (!str.starts_with("Welcome Moon and Star...") &&
		    !str.ends_with("Where Destiny is Made.")) {
			throw std::runtime_error(fmt::format("Handshake recv failed, {}", str));
		}
	}

	return true;
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
		g_utils->log(ERROR,
		             fmt::format("Error while trying to process server button, {}",
		                         e.what()));
	}
};
void serverConnection::join(int serverListIndex) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");
	try {
		join(g_game->serverListEntries[serverListIndex]);
	} catch (const std::exception &e) {
		g_utils->log(ERROR,
		             fmt::format("Error while trying to process server button, {}",
		                         e.what()));
	}
};
void serverConnection::join(std::string_view ip, unsigned int portStart) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");

	int customPortStart = portStart;
	int customPort = customPortStart;

	while (!connection.get()) {
		try {
			connection = std::make_shared<TCPConnection>(ip, customPort);
		} catch (const std::exception) {
		};
		customPort++;
		if (customPort - customPortStart >= 50) break;
		if (connection.get()) break;
	};

	if (!connection.get())
		throw std::runtime_error("Tried 50 other ports, failed.");

	valid = true;
	handshake();
}
void serverConnection::join(ServerListEntry serverListEntry) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");

	try {
		if (serverListEntry.versionMajor < 36) return;

		std::string ipName(fmt::format("{}.{}.{}.{}", serverListEntry.ip[3],
		                               serverListEntry.ip[2], serverListEntry.ip[1],
		                               serverListEntry.ip[0]));

		g_utils->log(
		    INFO, fmt::format("Connecting to {}:{}", ipName, serverListEntry.port));

		join(ipName, subRosaCustomPortStart);
	} catch (const std::exception &e) {
		g_utils->log(ERROR,
		             fmt::format("Error while trying to process server button, {}",
		                         e.what()));
	}
};
void serverConnection::join(std::shared_ptr<TCPConnection> con) {
	if (connectionCheck()) throw std::runtime_error("Connection already exists.");
	valid = true;
	connection = con;
};

void serverConnection::close() {
	if (connectionCheck() && connection->socketFD != -1) connection->close();
	g_utils->log(INFO, "Custom connection closed.");
}