#include "api.hpp"
#include "tcpSocket.hpp"

// event header
struct EventInfo {
	int tick;
	int type;
};

enum EventTypes_enum {
	EVENT_DRAWTEXT,
};

struct DrawTextEvent {
	char message[64];
	float x;
	float y;
	float scale;
	int flags;
	float r;
	float g;
	float b;
	float a;
};

union EventUnion {
	DrawTextEvent drawText;
	// other events...
};

struct Event {
	EventInfo info;
	EventUnion data;
};

class eventHandler {
 public:
	eventHandler(TCPConnection con) : serverConnection(con) {};
	eventHandler(eventHandler &&) = default;
	eventHandler(const eventHandler &) = default;
	eventHandler &operator=(eventHandler &&) = default;
	eventHandler &operator=(const eventHandler &) = default;
	~eventHandler() {};

	void processEvents();
 private:
	std::vector<Event> events;
    TCPConnection serverConnection;
};
