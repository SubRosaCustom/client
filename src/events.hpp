#include "api.hpp"
#include "networking/tcpSocket.hpp"
#include "networking/packetHandler.hpp"

#include <memory>

// event header
struct EventInfo {
	int tick;
	int type;
	int length;
};

enum EventTypes_enum {
	EVENT_HANDSHAKE,
	EVENT_DRAWTEXT,
};

struct DrawTextEvent {
	int messageLength;
	char message[64];
	float x;
	float y;
	float scale;
	float r;
	float g;
	float b;
	float a;
};

struct HandshakeEvent {
	char message[15];
	int magic;
};

union EventUnion {
	HandshakeEvent handshake;
	DrawTextEvent drawText;
	// other events...
};

struct Event {
	EventInfo info;
	EventUnion data;
};

class eventHandler {
 public:
	eventHandler() {};
	eventHandler(eventHandler &&) = default;
	eventHandler(const eventHandler &) = default;
	eventHandler &operator=(eventHandler &&) = default;
	eventHandler &operator=(const eventHandler &) = default;
	~eventHandler() {};

	void processEvents();
	void triggerEventHandshake();
 private:
};

inline std::shared_ptr<eventHandler> g_eventHandler;