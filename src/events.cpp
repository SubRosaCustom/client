#include "events.hpp"

void eventHandler::processEvents() {
	Event event;

	// try to read the event header
	if (serverConnection.recv((char *)&event.info, sizeof(EventInfo)) ==
	    sizeof(EventInfo)) {
		// now that we've read the events header read the actual event data
		switch (event.info.type) {
			case EVENT_DRAWTEXT:
				DrawTextEvent data = event.data.drawText;

				// read the draw text events info
				if (serverConnection.recv((char *)&data, sizeof(DrawTextEvent)) <
				    sizeof(DrawTextEvent)) {
					api::drawText(data.message, data.x, data.y, data.scale, data.flags,
					              data.r, data.g, data.b, data.a);
				}

				break;
		}
	}
}