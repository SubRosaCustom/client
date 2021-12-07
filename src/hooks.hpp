#pragma once

#include <memory>
#include <type_traits>

#ifdef _WIN32
#define SUBHOOK_STATIC
#endif

#include "subhook.h"

class hooks
{
public:
    hooks();
    subhook::Hook renderFrameHook;
    subhook::Hook drawHudHook;
    subhook::Hook drawTextHook;
    subhook::Hook drawMainMenuHook;
    subhook::Hook drawCreditsMenuHook;
};

inline std::unique_ptr<hooks> g_hooks;