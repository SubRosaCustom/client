#include "api.hpp"
#include "game.hpp"
#include <cstdarg>
#include <string>

namespace api
{
    void drawText(const char *text, float x, float y, float scale, int params, float r, float g, float b, float a, ...)
    {
        char buffer[256];
        va_list list;
        va_start(list, a);
        vsnprintf(buffer, 255, text, list);
#ifdef _WIN32
        g_game->drawTextFunc(buffer, x, y, scale, params, r, g, b, a);
#else
        g_game->drawTextFunc(buffer, params, x, y, scale, r, g, b, a);
#endif
        va_end(list);
    }
}