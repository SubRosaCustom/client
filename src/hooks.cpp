#include "structs.hpp"
#include "hooks.hpp"
#include "game.hpp"

#include <iostream>
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
        std::cout << stream.str() << std::endl;                                                                   \
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

int64_t drawText(char *text, int params, void* a, void* b, float x, float y, float scale, float red, float green, float blue, float alpha, void* c)
{
    subhook::ScopedHookRemove remove(&g_hooks->drawTextHook);

    auto ret = g_game->drawTextFunc(text, params | TEXT_SHADOW, a, b, x, y, scale, red, green, blue, alpha, c);
    return ret;
}

void createSound(float arg1, float arg2, int arg3, Vector3 *arg4)
{
    subhook::ScopedHookRemove remove(&g_hooks->createSoundHook);

    g_game->createSoundFunc(arg1, arg2, arg3, arg4);
}

int createParticle(float arg1, int arg2, Vector3 *arg3, Vector3 *arg4)
{
    subhook::ScopedHookRemove remove(&g_hooks->createParticleHook);

    g_game->createParticleFunc(arg1, arg2, arg3, arg4);
}

int drawMainMenu()
{
    subhook::ScopedHookRemove remove(&g_hooks->drawMainMenuHook);

    auto ret = g_game->drawMainMenuFunc();
    subhook::ScopedHookRemove removea(&g_hooks->drawTextHook);
    g_game->drawTextFunc("Custom Edition v0.0.1", TEXT_SHADOW | TEXT_CENTER, 0, 0, 512.f, 192.f, 16.f, 1, 1, 1, 1, 0);

    return ret;
}

int drawCreditsMenu()
{
    subhook::ScopedHookRemove remove(&g_hooks->drawCreditsMenuHook);

    auto ret = g_game->drawCreditsMenuFunc();
    subhook::ScopedHookRemove removea(&g_hooks->drawTextHook);
    g_game->drawTextFunc("Custom Edition", TEXT_SHADOW, 0, 0, 200.f, 64.f, 16.f, 0.75, 0.75, 0.75, 1, 0);
    g_game->drawTextFunc("noche", TEXT_SHADOW, 0, 0, 200.f, 96.f, 16.f, 1, 1, 1, 1, 0);
    g_game->drawTextFunc("AssBlaster", TEXT_SHADOW, 0, 0, 200.f, 112.f, 16.f, 1, 1, 1, 1, 0);

    return ret;
}

hooks::hooks()
{
    printf("satellite\n");
    INSTALL(renderFrame);
    INSTALL(drawHud);
    INSTALL(drawText);
    INSTALL(drawMainMenu);
    INSTALL(drawCreditsMenu);
    INSTALL(createSound);
    INSTALL(createParticle);
}