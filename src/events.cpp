#include "events.hpp"

#include "networking/serverConnection.hpp"

/*
 >00000000 00000000 68656C6F 007F0000< ........helo.... 00000000
 >C01E03B7 FE7F0000 2D5AD847 4A7F0000< ........-Z.GJ... 00000010
 >2AA30000 00000000 A0CEEF47 4A7F0000< *..........GJ... 00000020
 >E01E03B7 FE7F0000 A6CAE747 4A7F0000< ...........GJ... 00000030
 >C041B995 B0550000 00004843 00004843< .A...U....HC..HC 00000040
 >00008041 00000000 0000803F 0000803F< ...A.......?...? 00000050
 >0000803F 0000803F<                   ...?...?         00000060
 
  00000000  00 00 00 00 00 00 00 00  68 65 6C 69 00 00 00 00  ........heli....
  00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
  00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
  00000030  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
  00000040  00 00 00 00 00 00 00 00  00 00 80 41 00 00 80 41  ..........�A..�A
  00000050  00 00 80 41 00 00 00 00  00 00 80 3F 00 00 80 3F  ..�A......�?..�?
  00000060  00 00 80 3F 00 00 80 3F                           ..�?..�?
*/

void eventHandler::processEvents() {
	if (!g_serverConnection->connection.get()) return;
	Event event;

	int first = g_serverConnection->connection->recv((char *)&event.info,
	                                                 sizeof(EventInfo));
	// try to read the event header
	if (first == sizeof(EventInfo)) {
		// now that we've read the events header read the actual event data

		switch (event.info.type) {
			case EVENT_DRAWTEXT: {
				DrawTextEvent data = event.data.drawText;

				int re = g_serverConnection->connection->recv((char *)&data,
				                                              sizeof(DrawTextEvent));
				// read the draw text events info
				if (re <= sizeof(DrawTextEvent)) {

				spdlog::error("LAAAAAAAa {}", data.message);
					events.push_back(event);
				}

				break;
			}

			default:
				spdlog::error(
				             "Unknown event type, {}", event.info.type);
				break;
		}
	}
}