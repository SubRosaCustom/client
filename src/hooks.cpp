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

#define INSTALL(name)                                                                                             \
    if (!name##Hook.Install((void *)g_game->name##Func, (void *)::name, subhook::HookFlags::HookFlag64BitOffset)) \
    {                                                                                                             \
        ERROR_AND_EXIT("Hook %sHook failed to install", #name);                                                   \
    } \
    printf(#name " hooked!\n");

#define REMOVE_HOOK(name) subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

int64_t renderFrame(int64_t arg1, int64_t arg2, double* arg3)
{
	REMOVE_HOOK(renderFrame);

	auto ret = g_game->renderFrameFunc(arg1, arg2, arg3);
	return ret;
}

int64_t drawHud(int64_t arg1)
{
	REMOVE_HOOK(drawHud);

	printf("drawHud called\n");
	g_game->drawTextFunc((char*)"Custom Edition v0.0.1", TEXT_SHADOW | TEXT_CENTER, 0, 0, 0, 1, 1, 1, 1, 16.f, 192.f, 512.f);
	
	auto ret = g_game->drawHudFunc(arg1);
	return ret;
}

#ifdef _WIN32
int64_t drawText(char* text, int params, int a, int b, float x, float y, float scale, float red, float green, float blue, float alpha, int c)
#else
int64_t drawText(char* text, int params, int a, int b, float x, float y, float scale, float red, float green, float blue, float alpha, int c)
#endif
{
	REMOVE_HOOK(drawText);

	printf("DrawText %s, %#x, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %i\n", text, params, a, b, x, y, scale, red, green, blue, alpha, c);
#ifdef _WIN32
	auto ret = g_game->drawTextFunc(text, params, a, b, x, y, scale, red, green, blue, alpha, c);
	//auto ret = g_game->drawTextFunc(text, c, alpha, blue, green, red, scale, y, x, b, a, params);
#else
	auto ret = g_game->drawTextFunc(text, params, a, b, x, y, scale, red, green, blue, alpha, c);
#endif
	return ret;
}

void createSound(float arg1, float arg2, int arg3, Vector3* arg4)
{
	REMOVE_HOOK(createSound);

	g_game->createSoundFunc(arg1, arg2, arg3, arg4);
}

int createParticle(float arg1, int arg2, Vector3* arg3, Vector3* arg4)
{
	REMOVE_HOOK(createParticle);

	auto ret = g_game->createParticleFunc(arg1, arg2, arg3, arg4);
	return ret;
}

int drawMainMenu()
{
	REMOVE_HOOK(drawMainMenu);

	auto ret = g_game->drawMainMenuFunc();
	REMOVE_HOOK(drawText);
	g_game->drawTextFunc((char*)"Custom Edition v0.0.1", TEXT_SHADOW | TEXT_CENTER, 0, 0, 512.f, 192.f, 16.f, 1, 1, 1, 1, 0);

	return ret;
}

int drawCreditsMenu()
{
	REMOVE_HOOK(drawCreditsMenu);

	auto ret = g_game->drawCreditsMenuFunc();
	REMOVE_HOOK(drawText);
	g_game->drawTextFunc((char*)"Custom Edition", TEXT_SHADOW, 0, 0, 200.f, 64.f, 16.f, 0.75, 0.75, 0.75, 1, 0);
	g_game->drawTextFunc((char*)"noche", TEXT_SHADOW, 0, 0, 200.f, 96.f, 16.f, 1, 1, 1, 1, 0);
	g_game->drawTextFunc((char*)"AssBlaster", TEXT_SHADOW, 0, 0, 200.f, 112.f, 16.f, 1, 1, 1, 1, 0);

	return ret;
}

hooks::hooks()
{
	printf("Installing hooks...\n");
	//INSTALL(renderFrame);
	INSTALL(drawHud);
	//INSTALL(drawText);
	//INSTALL(drawMainMenu);
	//INSTALL(drawCreditsMenu);
	//INSTALL(createSound);
	//INSTALL(createParticle);
	printf("Hooks installed!\n");
}