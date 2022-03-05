#include "events.hpp"

#include "networking/serverConnection.hpp"
#include "utils/notifications.hpp"

void eventHandler::triggerEventHandshake() {
	if (!g_serverConnection->connectionCheck())
		throw std::runtime_error("Connection doesn't exist.");

	packetHandler handSend;
	handSend.write<int>(0);   // handshake type
	handSend.write<int>(16);  // length
	handSend.write<int>(subRosaCustomMagic);
	handSend.write<int>(customVersion[0]);
	handSend.write<int>(customVersion[1]);
	handSend.write<int>(customVersion[2]);

	if (g_serverConnection->connection->send(handSend.get()) <= 0)
		throw std::runtime_error("Handshake send failed.");
}

void eventHandler::processEvents() {
	if (!g_serverConnection->connection.get() ||
	    !g_serverConnection->connection->valid)
		return;
	Event event;
	char header[sizeof(EventInfo) * 2];
	std::memset(&header, 0, sizeof(header));

	int first =
	    g_serverConnection->connection->recv(header, sizeof(EventInfo) * 2);

	if (first != sizeof(EventInfo) * 2) return;

	packetHandler headerHandler(header);
	event.info.tick = headerHandler.read<int>();
	event.info.type = headerHandler.read<int>();
	event.info.length = headerHandler.read<int>();
	spdlog::debug("Packet Length: {}", event.info.length);

	char dat[event.info.length];
	std::memset(&dat, 0, sizeof(dat));
	int second = g_serverConnection->connection->recv(dat, event.info.length);
	if (second != event.info.length) return;

	packetHandler dataHandler(dat);
	switch (event.info.type) {
		case EVENT_HANDSHAKE: {
			HandshakeEvent data = event.data.handshake;

			dataHandler.readString(15).copy(data.message, 15);
			data.message[15] = '\0';
			data.magic = dataHandler.read<int>();

			spdlog::info("Handshake result {}", data.message);

			if (std::strstr(data.message, "MoonStarDestiny") &&
			    data.magic == subRosaCustomMagic)
				spdlog::info("Success handshake");

			break;
		}
		case EVENT_DRAWTEXT: {
			DrawTextEvent data = event.data.drawText;

			event.data.drawText.messageLength = dataHandler.read<int>();

			dataHandler.readString(event.data.drawText.messageLength)
			    .copy(event.data.drawText.message, event.data.drawText.messageLength);
			event.data.drawText.message[event.data.drawText.messageLength] = '\0';
			event.data.drawText.message[64] = '\0';

			spdlog::info("{}", event.data.drawText.message);

			event.data.drawText.x = dataHandler.read<float>();
			event.data.drawText.y = dataHandler.read<float>();
			event.data.drawText.scale = dataHandler.read<float>();
			event.data.drawText.r = dataHandler.read<float>();
			event.data.drawText.g = dataHandler.read<float>();
			event.data.drawText.b = dataHandler.read<float>();
			event.data.drawText.a = dataHandler.read<float>();

			api::addText(event.data.drawText.message, event.data.drawText.x,
			             event.data.drawText.y, event.data.drawText.scale, 600,
			             event.data.drawText.r, event.data.drawText.g,
			             event.data.drawText.b);
			break;
		}

		default:
			// spdlog::error("Unknown event type, {}", event.info.type);
			break;
	}
}