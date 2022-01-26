#include "api.hpp"
#include "tcpSocket.hpp"

#include <memory>

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
	eventHandler() {};
	eventHandler(eventHandler &&) = default;
	eventHandler(const eventHandler &) = default;
	eventHandler &operator=(eventHandler &&) = default;
	eventHandler &operator=(const eventHandler &) = default;
	~eventHandler() {};

	void processEvents();
	std::vector<Event> events;
 private:
};

inline std::shared_ptr<eventHandler> g_eventHandler;