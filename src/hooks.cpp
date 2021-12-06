#include "hooks.hpp"
#include "game.hpp"

#include <sstream>

#ifdef _WIN32
#include <Windows.h>
#include <Psapi.h>
#elif __linux__
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#else
#define INSTALL(name)                                                                                             \
    if (!name##Hook.Install((void *)g_game->name##Func, (void *)::name, subhook::HookFlags::HookFlag64BitOffset)) \
    {                                                                                                             \
        std::ostringstream stream;                                                                                \
        \ 
		stream                                                                                                    \
            << "Hook " << #name "Hook"                                                                            \
            << " failed to install";                                                                              \
        std::cout << stream.str() << std::endl;                                                                         \
        throw std::runtime_error(stream.str());                                                                   \
    }
#endif

int64_t renderFrame(int64_t arg1, int64_t arg2, double *arg3)
{
    subhook::ScopedHookRemove remove(&g_hooks->renderFrameHook);

    auto ret = g_game->renderFrameFunc(arg1, arg2, arg3);
    return ret;
}

int64_t drawHud(int64_t arg1)
{
    subhook::ScopedHookRemove remove(&g_hooks->drawHudHook);

    auto ret = g_game->drawHudFunc(arg1);
    return ret;
}
#include <iostream>
int64_t drawTextShadow(char *text, int32_t b, char c, float x, float y, float scale, float red, float green, float blue, float alpha)
{
    subhook::ScopedHookRemove remove(&g_hooks->drawTextShadowHook);

    g_game->drawTextShadowFunc(text, b, c, x+ 1, y+1, scale, 0, 0, 0, alpha);
    auto ret = g_game->drawTextShadowFunc(text, b, c, x, y, scale, red, green, blue, alpha);
    return ret;
}

hooks::hooks()
{
    printf("satellite\n");
    INSTALL(renderFrame);
    INSTALL(drawHud);
    INSTALL(drawTextShadow);
}