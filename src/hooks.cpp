#include "hooks.hpp"
#include "game.hpp"

#include <sstream>

#ifdef _WIN32
#else
#define INSTALL(name)                                                                               \
    if (!name##Hook.Install((void *)g_game->name##Func, (void *)::name, subhook::HookFlags::HookFlag64BitOffset)) \
    {                                                                                               \
        std::ostringstream stream;                                                                  \
        \ 
		stream                                                                                      \
            << "Hook " << #name "Hook"                                                              \
            << " failed to install";                                                                \
        throw std::runtime_error(stream.str());                                                     \
    }
#endif

int64_t renderFrame(int64_t arg1, int64_t arg2, double *arg3)
{
    subhook::ScopedHookRemove remove(&g_hooks->renderFrameHook);

    auto ret = g_game->renderFrameFunc(arg1, arg2, arg3);

    return ret;
}

hooks::hooks()
{
    INSTALL(renderFrame);
}