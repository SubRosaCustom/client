#include "api.hpp"

#include <cstdarg>
#include <string>

#include "game.hpp"
#include "hooks.hpp"

#define REMOVE_HOOK(name) \
	subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

namespace api {
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a) {
#ifdef _WIN32
	g_game->drawTextFunc((char*)text.data(), x, y, scale, params, r, g, b, a);
#else
	g_game->drawTextFunc((char*)text.data(), params, x, y, scale, r, g, b, a, 0);
#endif
}
}  // namespace api