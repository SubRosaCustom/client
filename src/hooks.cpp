#include "hooks.hpp"

#include <assert.h>

#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>

#include "api.hpp"
#include "console.hpp"
#include "fmt/format.h"
#include "game.hpp"
#include "gui.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "memoryEditor.hpp"
#include "settings.hpp"
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

	bool isAnyGuiActive = false;
	for (auto &&gui : activeGuiList) {
		if (gui->isOpen) {
			isAnyGuiActive = true;
			break;
		}
	}

	if (result && ImGui_ImplSDL2_ProcessEvent(event) && isAnyGuiActive)
		event->type = 0;

	for (auto &&gui : activeGuiList) {
		gui->handleKeyPress(event);
	}

	return result;
};

SDL_Window *sdlWindow = nullptr;

void swapWindow(SDL_Window *window) {
	REMOVE_HOOK(swapWindow);

	sdlWindow = window;

	[[maybe_unused]] static const auto once = [window]() noexcept {
		assert(ImGui_ImplSDL2_InitForOpenGL(window, nullptr) &&
		       "Unable to init ImGui, ImGui_ImplSDL2_InitForOpenGL");
		assert(ImGui_ImplOpenGL3_Init() &&
		       "Unable to init ImGui, ImGui_ImplOpenGL3_Init");

		api::init();

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		g_utils->log(INFO, "ImGui Initialized!");

		return true;
	}();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();

	auto isAnyGuiActive = false;
	auto &io = ImGui::GetIO();
	io.MouseDrawCursor = false;
	for (auto &&gui : activeGuiList) {
		gui->draw();
		if (!isAnyGuiActive && gui->isOpen) {
			io.MouseDrawCursor = true;
			isAnyGuiActive = true;
		}
	}

	// if more than 10 events have passed since last event or smth
	if (*g_game->numEventsNeedSync > 10) {
		ImGuiIO &io = ImGui::GetIO();
		ImGuiWindowFlags window_flags =
		    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		    ImGuiWindowFlags_NoNav;

		const float PAD = 10.0f;
		const ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImVec2 work_pos =
		    viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = work_pos.x + work_size.x - PAD - (work_size.x / 20);
		window_pos.y = work_pos.y + PAD;
		window_pos_pivot.x = 1.0f;
		window_pos_pivot.y = 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;

		ImGui::SetNextWindowBgAlpha(0.6f);
		if (ImGui::Begin("Server Connection", (bool *)true, window_flags)) {
			ImGui::Text(fmt::format("Server hasn't responded\nfor {:.2f} seconds",
			                        *g_game->numEventsNeedSync / 62.5)
			                .c_str());
		}
		ImGui::End();
	}

	api::setDrawList(ImGui::GetBackgroundDrawList());

	if (g_settings->get_var<bool>("small_chat")) {
		float x = 10.f;
		float y = 500.f;
		auto innerPadding = 2.f;
		float messageOffset = innerPadding * 3.f + 4;
		for (size_t i = 0; i <= *g_game->amountOfChatMessages; i++) {
			auto message = g_game->chatMessages[i];
			if (message.timer > 0) {
				auto gb = 1.f;
				if (message.type == 6) gb = 0.f;
				auto size = ImGui::CalcTextSize(message.message);

				api::drawRectFilledImGui(
				    ImVec2(x - innerPadding, y - innerPadding),
				    ImVec2(size.x + innerPadding * 3.f, size.y + innerPadding * 3.f),
				    ImColor(0.5f, 0.5f, 0.5f, message.timer / 200.f), 4.f,
				    ImDrawFlags_RoundCornersAll);
				api::drawTextImGui(message.message, x, y, 13.f, 0.f, 1.f, gb, gb,
				                   message.timer / 200.f);
				y = y + size.y + messageOffset;
			}
		}
	}

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	g_game->swapWindowFunc(window);
};

void mouseRelativeUpdate(Mouse *mouse) {
	REMOVE_HOOK(mouseRelativeUpdate);

	auto flags = SDL_GetWindowFlags(sdlWindow ? sdlWindow : 0);
	if ((flags & SDL_WINDOW_SHOWN) && (flags & SDL_WINDOW_INPUT_FOCUS) &&
	    (flags & SDL_WINDOW_MOUSE_FOCUS))
		g_game->mouseRelativeUpdateFunc(mouse);
	else {
		g_game->mouse->deltaX = 0;
		g_game->mouse->deltaY = 0;
		g_game->mouse->isLeftMouseDown = 0;
		g_game->mouse->isRightMouseDown = 0;
		g_game->mouse->mouseInputs = 0;
	}

	for (auto &&gui : activeGuiList) {
		if (gui->isOpen) {
			g_game->mouse->deltaX = 0;
			g_game->mouse->deltaY = 0;
			g_game->mouse->isLeftMouseDown = 0;
			g_game->mouse->isRightMouseDown = 0;
			g_game->mouse->mouseInputs = 0;
			break;
		}
	}
}

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

	int editedParams = params;
	// g_utils->log(INFO, fmt::format("{} {:#x}, {:#x}", text,
	//                                RETURN_ADDRESS() - g_game->getBaseAddress(),
	//                                g_game->createNewspaperText -
	//                                    g_game->getBaseAddress()));

	// If it's not a newspaper, memo, street sign, chat message add a shadow
	if (RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x14916c) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x149b6b) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x5603b) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x149707))
		editedParams |= TEXT_SHADOW;

	// dont render text if its small chat
	if (g_settings->get_var<bool>("small_chat") &&
	    RETURN_ADDRESS() == g_game->getBaseAddress() + WIN_LIN(TODO, 0xd90f3)) {
		return 0;
	}

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

	api::drawText("CE Contributors", 200.f, 144.f, 16.f, TEXT_SHADOW, 0.75, 0.75,
	              0.75, 1);
	api::drawText("checkraisefold", 200.f, 176.f, 16.f, TEXT_SHADOW, 1, 1, 1, 1);

	return ret;
}

int drawOptionsMenu() {
	REMOVE_HOOK(drawOptionsMenu);

	auto ret = g_game->drawOptionsMenuFunc();
	api::drawText("Custom Edition Settings", 230.f, 600.f, 16.f, TEXT_SHADOW, 1,
	              1, 1, 1);

	// *(float*)(g_game->getBaseAddress() + WIN_LIN(TODO, 0x6a8a76c0)) = 0; //
	// item length
	// *(float*)(g_game->getBaseAddress() + WIN_LIN(TODO, 0x6a8a76c4)) = 0; //
	// item height
	*(float *)(g_game->getBaseAddress() + WIN_LIN(TODO, 0x6a8a76b8)) =
	    5.f;  // next item x offset from 0
	*(float *)(g_game->getBaseAddress() + WIN_LIN(TODO, 0x6a8a76bc)) =
	    350.f;  // next item y offset from 0

	static int smallChatMessages = (int)g_settings->get_var<bool>("small_chat");
	g_game->drawMenuTextFunc("Custom Edition Settings");
	if (g_game->drawMenuCheckboxFunc("Small Chat Messages", &smallChatMessages)) {
		g_settings->set_var("small_chat", (bool)smallChatMessages);
		g_settings->save();
	}

	return ret;
}

int renderPNG(int a, int b, char c, float d, float e, float f, float g) {
	REMOVE_HOOK(renderPNG);
	// g_utils->log(INFO, fmt::format("gets called {}, {}, {}, {}, {}, {}, {}", a,
	// b, c, d, e, f, g));

	return 0;
}

hooks::hooks() {
	ImGui::CreateContext();

	int w, h;
	SDL_GetWindowSize(0, &w, &h);

	g_settings->init();

	g_console = std::make_unique<console>(
	    "Console", false, ImVec2(0, 0), ImVec2(h, w),
	    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
	        ImGuiWindowFlags_NoResize);

	g_memoryEditor =
	    std::make_unique<memoryEditor>("Memory Editor", false, ImVec2(500, 200),
	                                   ImVec2(400, 300), ImGuiWindowFlags_None);
}

void hooks::install() {
	INSTALL(swapWindow);
	INSTALL(pollEvent);
	INSTALL(mouseRelativeUpdate);
	INSTALL(renderFrame);
	INSTALL(drawHud);
	INSTALL(drawText);
	INSTALL(drawMainMenu);
	INSTALL(drawCreditsMenu);
	INSTALL(drawOptionsMenu);
	// INSTALL(renderPNG);

	g_utils->log(INFO, "Hooks installed!");
}