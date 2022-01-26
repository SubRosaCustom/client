#include "events.hpp"

#include "serverConnection.hpp"

/*
00000000  30 30 30 30 30 30 30 30  30 30 30 30 30 30 68 65  00000000000000he
00000010  6C 6C 6F 20 77 6F 72 6C  64 30 30 30 30 30 63 38  llo world00000c8
00000020  30 30 30 30 30 63 38 30  30 30 30 30 31 30 30 30  00000c8000001000
00000030  30 30 30 30 63 30 30 30  30 30 30 31 30 30 30 30  0000c00000010000
00000040  30 30 31 30 30 30 30 30  30 31 30 30 30 30 30 30  0010000001000000
00000050  31                                                1
*/
/*
 >00000000 00000000 68656C6F 007F0000< ........helo.... 00000000
 >C01E03B7 FE7F0000 2D5AD847 4A7F0000< ........-Z.GJ... 00000010
 >2AA30000 00000000 A0CEEF47 4A7F0000< *..........GJ... 00000020
 >E01E03B7 FE7F0000 A6CAE747 4A7F0000< ...........GJ... 00000030
 >C041B995 B0550000 00004843 00004843< .A...U....HC..HC 00000040
 >00008041 00000000 0000803F 0000803F< ...A.......?...? 00000050
 >0000803F 0000803F<                   ...?...?         00000060
*/

void eventHandler::processEvents() {
	if (!g_serverConnection->connection.get()) return;
	Event event;

	int first = g_serverConnection->connection->recv((char *)&event.info,
	                                                 sizeof(EventInfo));

	// try to read the event header
	if (first == sizeof(EventInfo)) {
		// now that we've read the events header read the actual event data

		g_utils->log(INFO, fmt::format("Unknaefaef, {}", event.info.type));
		switch (event.info.type) {
			case EVENT_DRAWTEXT: {
				DrawTextEvent data = event.data.drawText;

				int re = g_serverConnection->connection->recv((char *)&data,
				                                              sizeof(DrawTextEvent));
				// read the draw text events info
				if (re <= sizeof(DrawTextEvent)) {
					events.push_back(event);
				}
				g_utils->log(INFO, fmt::format("re {}, {}", re, sizeof(DrawTextEvent)));

				break;
			}

			default:
				g_utils->log(ERROR,
				             fmt::format("Unknown event type, {}", event.info.type));
				break;
		}
	}
	g_utils->log(INFO, fmt::format("when {}", first));
}