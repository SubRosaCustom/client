#include "api.hpp"

#include <cstdarg>
#include <string>

#include "game.hpp"
#include "hooks.hpp"

#define REMOVE_HOOK(name) \
	subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

namespace api {
void drawText(const char *text, float x, float y, float scale, int params,
              float r, float g, float b, float a, ...) {
	REMOVE_HOOK(drawText);
	char buffer[256];
	va_list list;
	va_start(list, a);
	vsnprintf(buffer, 256, text, list);
	va_end(list);
#ifdef _WIN32
	g_game->drawTextFunc(buffer, x, y, scale, params, r, g, b, a);
#else
	g_game->drawTextFunc(buffer, params, x, y, scale, r, g, b, a);
#endif
}
}  // namespace api