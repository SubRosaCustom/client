#pragma once

#include <memory>
#include <type_traits>

#include "game.hpp"

#ifdef _WIN32
#define SUBHOOK_STATIC
#endif

#include "subhook.h"

class hooks {
 public:
	hooks();
	void install();

	subhook::Hook swapWindowHook;
	subhook::Hook pollEventHook;
	subhook::Hook sdlDelayHook;
	subhook::Hook mouseRelativeUpdateHook;
	subhook::Hook renderFrameHook;
	subhook::Hook drawHudHook;
	subhook::Hook drawTextHook;
	subhook::Hook drawMainMenuHook;
	subhook::Hook drawCreditsMenuHook;
	subhook::Hook drawOptionsMenuHook;
	subhook::Hook drawMenuButtonHook;
	subhook::Hook serverEventLoopHook;
	subhook::Hook testHookHook;
	subhook::Hook renderPNGHook;
	subhook::Hook createTextureHook;
	subhook::Hook loadCMOHook;
	subhook::Hook loadCMCHook;
};

inline std::unique_ptr<hooks> g_hooks;