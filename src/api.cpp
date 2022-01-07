#include "api.hpp"

#include <cstdarg>
#include <string>

#include "game.hpp"

namespace api {
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a) {
#ifdef _WIN32
	g_game->drawTextFunc((char*)text.data(), x, y, scale, params, r, g, b, a, 0);
#else
	g_game->drawTextFunc((char*)text.data(), params, 0, 0, x, y, scale, r, g, b, a, 0);
#endif
}
}  // namespace api