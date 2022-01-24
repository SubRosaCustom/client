#include "hooks.hpp"

#include <iostream>
#include <sstream>

#include "api.hpp"
#include "game.hpp"
#include "structs.hpp"
#include "tcpSocket.hpp"
#include "utils.hpp"

#ifdef _WIN32
#include <Psapi.h>
#include <Windows.h>
#elif __linux__
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <cmath>
#include <ctime>

#include "fmt/format.h"

extern "C" void FASTCALL pushVarArgs(void *addr, long long count);
extern "C" void FASTCALL clearStack(long long count);

#define INSTALL(name)                                                    \
	if (!name##Hook.Install((void *)g_game->name##Func, (void *)::name,    \
	                        subhook::HookFlags::HookFlag64BitOffset)) {    \
		ERROR_AND_EXIT(fmt::format("Hook {}Hook failed to install", #name)); \
	}                                                                      \
	g_utils->log(DEBUG, #name " hooked!")

#define REMOVE_HOOK(name) \
	subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

int64_t renderFrame(int64_t arg1, int64_t arg2, double *arg3) {
	REMOVE_HOOK(renderFrame);

	auto ret = g_game->renderFrameFunc(arg1, arg2, arg3);
	return ret;
}

int64_t drawHud(int64_t arg1) {
	REMOVE_HOOK(drawHud);

	auto ret = g_game->drawHudFunc(arg1);
	return ret;
}


#ifdef _WIN32
int64_t drawText(char *text, float x, float y, float scale, int params,
                 float red, float green, float blue, float alpha, ...)
#else
int64_t drawText(char *text, int params, int a, int b, float x, float y,
                 float scale, float red, float green, float blue, float alpha,
                 void *c)
#endif
{
	REMOVE_HOOK(drawText);

	// Generating game..., 				0x14d167
	// Cancel, 							0x2cace
	// Generating..., 					0x14c063
	// Generating level..., 			0x14d09e
	// Generating game..., 				0x14d167

	// [CHAT] 							0x5603b

	int editedParams = params;
	// g_utils->log(INFO, fmt::format("{} {:#x}, {:#x}", text,
	//    RETURN_ADDRESS() - g_game->getBaseAddress(),
	//    g_game->createNewspaperText -
	//    g_game->getBaseAddress()));

	// If it's not a newspaper, memo, street sign, add a shadow
	if (RETURN_ADDRESS() != g_game->getBaseAddress() + 0x14916c &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + 0x149b6b &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + 0x149707)
		editedParams |= TEXT_SHADOW;

// never do shit before this, stack corruption then sex
#ifdef _WIN32
	std::string_view textStr = text;
	auto argCount = std::count(
	    textStr.begin(), textStr.end(),
	    '/');  // this will break if alex uses a / escape or some shit (lol)
	if (argCount > 0) {
		pushVarArgs(&alpha, static_cast<long long>(argCount));
	}
	auto ret = g_game->drawTextFunc(text, x, y, scale, editedParams, red, green,
	                                blue, alpha);

	if (argCount > 0) clearStack(static_cast<long long>(argCount));
#else
	auto ret = g_game->drawTextFunc(text, editedParams, a, b, x, y, scale, red,
	                                green, blue, alpha, c);
#endif
	// printf("DrawText %s, %#x, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f,
	// %.2f, %i\n", text, params, a, b, x, y, scale, red, green, blue, alpha, c);
	return ret;
}

void createSound(float arg1, float arg2, int arg3, Vector3 *arg4) {
	REMOVE_HOOK(createSound);

	g_game->createSoundFunc(arg1, arg2, arg3, arg4);
}

int createParticle(float arg1, int arg2, Vector3 *arg3, Vector3 *arg4) {
	REMOVE_HOOK(createParticle);

	auto ret = g_game->createParticleFunc(arg1, arg2, arg3, arg4);
	return ret;
}

int drawMainMenu() {
	REMOVE_HOOK(drawMainMenu);

	auto ret = g_game->drawMainMenuFunc();

	api::drawText("Custom Edition v0.0.1", 512.f, 192.f, 16.f,
	              TEXT_SHADOW | TEXT_CENTER, 1, 1, 1, 1);

	return ret;
}

int drawCreditsMenu() {
	REMOVE_HOOK(drawCreditsMenu);

	auto ret = g_game->drawCreditsMenuFunc();

	api::drawText("Custom Edition", 200.f, 64.f, 16.f, TEXT_SHADOW, 0.75, 0.75,
	              0.75, 1);
	api::drawText("noche", 200.f, 96.f, 16.f, TEXT_SHADOW, 1, 1, 1, 1);
	api::drawText("AssBlaster", 200.f, 112.f, 16.f, TEXT_SHADOW, 1, 1, 1, 1);

	api::drawText("CE Contributors", 200.f, 144.f, 16.f, TEXT_SHADOW, 0.75, 0.75, 0.75, 1);
	api::drawText("checkraisefold", 200.f, 176.f, 16.f, TEXT_SHADOW, 1, 1, 1, 1);

	return ret;
}

void hooks::install() {
	g_utils->log(INFO, "Installing hooks...");
	
	INSTALL(renderFrame);
	INSTALL(drawHud);
	INSTALL(drawText);
	INSTALL(drawMainMenu);
	INSTALL(drawCreditsMenu);

	g_utils->log(INFO, "Hooks installed!");
}
