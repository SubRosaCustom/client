#include "hooks.hpp"

#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>

#include "api.hpp"
#include "game.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
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

#include <assert.h>

#include "console.hpp"
#include "fmt/format.h"
#include "gui.hpp"

std::vector<gui *> activeGuiList;

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

int pollEvent(SDL_Event *event) {
	REMOVE_HOOK(pollEvent)

	const auto result = g_game->pollEventFunc(event);

	if (event->type == SDL_WINDOWEVENT &&
	    event->window.event == SDL_WINDOWEVENT_RESIZED) {
		bool isAnyGuiActive = false;
		for (auto &&gui : activeGuiList) {
			gui->onResize(ImVec2(event->window.data1, event->window.data2));
		}
	}

	ImGuiIO &io = ImGui::GetIO();
	io.MouseDrawCursor = false;

	bool isAnyGuiActive = false;
	for (auto &&gui : activeGuiList) {
		if (gui->isOpen) {
			isAnyGuiActive = true;
			io.MouseDrawCursor = true;
		}
	}

	if (result && ImGui_ImplSDL2_ProcessEvent(event) && isAnyGuiActive)
		event->type = 0;

	for (auto &&gui : activeGuiList) {
		gui->handleKeyPress(event);
	}

	return result;
};

void swapWindow(SDL_Window *window) {
	REMOVE_HOOK(swapWindow);

	[[maybe_unused]] static const auto once = [window]() noexcept {
		assert(ImGui_ImplSDL2_InitForOpenGL(window, nullptr) &&
		       "Unable to init ImGui, ImGui_ImplSDL2_InitForOpenGL");
		assert(ImGui_ImplOpenGL3_Init() &&
		       "Unable to init ImGui, ImGui_ImplOpenGL3_Init");

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		g_utils->log(INFO, "ImGui Initialized!");

		return true;
	}();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);

	ImGui::NewFrame();

	g_console->draw();

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	g_game->swapWindowFunc(window);
};

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

int createNewspaperText(int itemID, int textureID) {
	REMOVE_HOOK(createNewspaperText);

	auto ret = g_game->createNewspaperTextFunc(itemID, textureID);
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

int unkTest(int a, int b, char c, float d, float e, float f, float g) {
	REMOVE_HOOK(unkTest);
	// g_utils->log(INFO, fmt::format("gets called {}, {}, {}, {}, {}, {}, {}", a,
	// b, c, d, e, f, g));

	return 0;
}

int64_t createStreetSignText(int32_t street, int32_t textureID) {
	REMOVE_HOOK(createStreetSignText);

	auto ret = g_game->createStreetSignTextFunc(street, textureID);
	return ret;
}

hooks::hooks() {
	int w, h;
	SDL_GetWindowSize(0, &w, &h);

	g_console = std::make_unique<console>(
	    "Console", false, ImVec2(0, 0), ImVec2(h, w),
	    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
	        ImGuiWindowFlags_NoResize);
}

void hooks::install() {
	ImGui::CreateContext();

	INSTALL(swapWindow);
	INSTALL(pollEvent);
	INSTALL(renderFrame);
	INSTALL(drawHud);
	INSTALL(drawText);
	INSTALL(drawMainMenu);
	INSTALL(drawCreditsMenu);
	INSTALL(createNewspaperText);
	INSTALL(createStreetSignText);
	INSTALL(unkTest);

	g_utils->log(INFO, "Hooks installed!");
}