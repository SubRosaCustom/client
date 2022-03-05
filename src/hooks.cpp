#include "hooks.hpp"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <assert.h>

#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>

#include "api.hpp"
#include "events.hpp"
#include "fmt/format.h"
#include "game.hpp"
#include "gui/chat.hpp"
#include "gui/console.hpp"
#include "gui/fontData.hpp"
#include "gui/gui.hpp"
#include "gui/memoryEditor.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "networking/serverConnection.hpp"
#include "networking/tcpSocket.hpp"
#include "structs.hpp"
#include "utils/settings.hpp"
#include "utils/utils.hpp"

std::vector<gui *> activeGuiList;

extern "C" void FASTCALL pushVarArgs(void *addr, long long count);
extern "C" void FASTCALL clearStack(long long count);

#define INSTALL(name)                                                     \
	if (!name##Hook.Install((void *)g_game->name##Func, (void *)::name,     \
	                        subhook::HookFlags::HookFlag64BitOffset)) {     \
		ERROR_AND_EXIT(                                                       \
		    spdlog::fmt_lib::format("Hook {}Hook failed to install", #name)); \
	}                                                                       \
	spdlog::debug(#name " hooked!");

#define REMOVE_HOOK(name) \
	subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

int pollEvent(SDL_Event *event) {
	REMOVE_HOOK(pollEvent)

	const auto result = g_game->pollEventFunc(event);

	if (event->type == SDL_WINDOWEVENT &&
	    event->window.event == SDL_WINDOWEVENT_RESIZED) {
		for (auto &&gui : activeGuiList) {
			gui->onResize(ImVec2(event->window.data1, event->window.data2));
		}
	}

	bool shouldDisableMouse = false;
	for (auto &&gui : activeGuiList) {
		if (gui->isOpen && gui->disableMouse) {
			shouldDisableMouse = true;
			break;
		}
	}

	if (result && ImGui_ImplSDL2_ProcessEvent(event) && shouldDisableMouse)
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

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		spdlog::info("ImGui Initialized!");

		return true;
	}();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();

	auto isAnyGuiActive = false;
	auto &io = ImGui::GetIO();
	auto &style = ImGui::GetStyle();
	io.MouseDrawCursor = false;
	for (auto &&gui : activeGuiList) {
		gui->draw();
		if (!isAnyGuiActive && gui->isOpen && gui->disableMouse) {
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

		style.Alpha = *g_game->numEventsNeedSync / 62.5f;
		ImGui::SetNextWindowBgAlpha(0.6f);
		if (ImGui::Begin("Server Connection", (bool *)true, window_flags)) {
			ImGui::Text(
			    spdlog::fmt_lib::format("Server hasn't responded\nfor {:.2f} seconds",
			                            *g_game->numEventsNeedSync / 62.5)
			        .c_str());
		}
		ImGui::End();
		style.Alpha = 1.f;
	}

	api::setDrawList(ImGui::GetBackgroundDrawList());

	for (auto &&event : g_eventHandler->events) {
		switch (event.info.type) {
			case EVENT_DRAWTEXT:
				DrawTextEvent data = event.data.drawText;
				// spdlog::info(
				//  "barbara {}, {}, {}, {}, {}, {}, {}, {}, {}",
				//  data.message, data.x, data.y, data.scale,
				//  data.flags, data.r, data.g, data.b, data.a);

				api::drawTextImGui(data.message, data.x, data.y, data.scale, data.flags,
				                   data.r, data.g, data.b, data.a);

				break;
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
		if (gui->isOpen && gui->disableMouse) {
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
	// spdlog::info("{} {:#x}, {:#x}", text,
	//  RETURN_ADDRESS() - g_game->getBaseAddress(),
	//  g_game->createNewspaperText - g_game->getBaseAddress());

	// If it's not a newspaper, memo, street sign, chat message add a shadow
	if (RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x14916c) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x149b6b) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x5603b) &&
	    RETURN_ADDRESS() != g_game->getBaseAddress() + WIN_LIN(TODO, 0x149707))
		editedParams |= TEXT_SHADOW;

	// dont render chat message text if small chat is enabled
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

int drawMainMenu() {
	REMOVE_HOOK(drawMainMenu);

	auto ret = g_game->drawMainMenuFunc();

	api::drawText(
	    spdlog::fmt_lib::format("Custom Edition v{}.{}.{}", customVersion[0],
	                            customVersion[1], customVersion[2]),
	    512.f, 192.f, 16.f, TEXT_SHADOW | TEXT_CENTER, 1, 1, 1, 1);

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
	static int expFPSUncap = (int)g_settings->get_var<bool>("exp_fps_uncap");
	g_game->drawMenuTextFunc((char *)"Custom Edition Settings");
	if (g_game->drawMenuCheckboxFunc((char *)"Small Chat Messages",
	                                 &smallChatMessages)) {
		g_settings->set_var("small_chat", (bool)smallChatMessages);
		g_settings->save();
	}
	if (g_game->drawMenuCheckboxFunc((char *)"FPS Uncap (Experimental!)",
	                                 &expFPSUncap)) {
		g_settings->set_var("exp_fps_uncap", (bool)expFPSUncap);
		g_settings->save();
	}

	return ret;
}

int renderPNG(int a, int b, char c, float d, float e, float f, float g) {
	REMOVE_HOOK(renderPNG);
	// spdlog::info("gets called {}, {}, {}, {}, {}, {}, {}", a,
	// b, c, d, e, f, g));

	return 0;
}

int drawMenuButton(char *text) {
	REMOVE_HOOK(drawMenuButton);

	auto ret = g_game->drawMenuButtonFunc(text);
	if (ret) {
		try {
			std::string t(text);
			if (t.starts_with("Server ")) {
				t = t.substr(7, 3);

				g_serverConnection->join(std::stoi(t, nullptr, 0));
				if (g_serverConnection.get()) {
					spdlog::info("Custom socket connected!");
				}
			} else if (t.starts_with("Exit Gam")) {
				spdlog::info("Custom socket destroying...");
				if (g_serverConnection.get() && g_serverConnection->valid) {
					g_serverConnection->close();
					g_serverConnection = nullptr;
				}
				// *g_game->numEventsNeedSync = 0;
			}
		} catch (const std::exception &e) {
			spdlog::error("Error while trying to process server button, {}",
			              e.what());
		}
	}

	return ret;
}

void serverEventLoop(void) {
	REMOVE_HOOK(serverEventLoop);

	if (g_serverConnection.get() && g_eventHandler.get()) {
		g_eventHandler->processEvents();
	}

	g_game->serverEventLoopFunc();
}

int testHook(void) {
	REMOVE_HOOK(testHook);
	return 0;
}

hooks::hooks() {
	ImGui::CreateContext();
	auto &io = ImGui::GetIO();

	// font setup
	{
		io.Fonts->ClearFonts();

		ImFontConfig font_cfg = ImFontConfig();
		font_cfg.OversampleH = font_cfg.OversampleV = 2;
		font_cfg.PixelSnapH = false;
		if (font_cfg.SizePixels <= 0.0f) font_cfg.SizePixels = 26.0f * 1.0f;

		font_cfg.EllipsisChar = (ImWchar)0x0085;
		font_cfg.GlyphOffset.y =
		    1.0f *
		    std::floor(font_cfg.SizePixels / 26.0f);  // Add +1 offset per 14 units

		static const ImWchar ranges[] = {
		    0x0020,
		    0x00FF,  // Basic Latin + Latin Supplement
		    0,
		};
		font_cfg.GlyphRanges = ranges;
		const ImWchar *glyph_ranges = font_cfg.GlyphRanges;

		io.Fonts->AddFontDefault();
		std::strcpy(font_cfg.Name,
		            spdlog::fmt_lib::format("OpenSans-Regular.ttf, {}px",
		                                    (int)font_cfg.SizePixels)
		                .c_str());
		io.FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
		    rockwell_compressed_data_base85, font_cfg.SizePixels, &font_cfg,
		    glyph_ranges);
		io.FontDefault->Scale = 0.7f;
	};
	// theme setup
	{

	};

	g_settings->init();

	g_notificationManager = std::make_unique<notificationManager>();
	g_console_options = std::make_unique<console_options>();
	g_console = std::make_unique<ImTerm::terminal<terminal_helper>>(
	    *g_console_options, "Sub Rosa: Custom Console");
	g_console->set_min_log_level(ImTerm::message::severity::info);

	spdlog::default_logger()->sinks().push_back(g_console->get_terminal_helper());

	g_memoryEditor = std::make_unique<memoryEditor>(
	    "Memory Editor", GuiFlags_DisablesMouse, ImVec2(500, 200),
	    ImVec2(400, 300), ImGuiWindowFlags_None);
	g_chatWindow = std::make_unique<chatWindow>(
	    "Chat Window", GuiFlags_None | GuiFlags_OpenOnStart, ImVec2(500, 200),
	    ImVec2(400, 300), ImGuiWindowFlags_None);

	g_serverConnection = std::make_unique<serverConnection>();
	g_eventHandler = std::make_unique<eventHandler>();
}

void sdlDelay(Uint32 a) {
	REMOVE_HOOK(sdlDelay);

	if (!g_settings->get_var<bool>("exp_fps_uncap")) g_game->sdlDelayFunc(a);
}

void hooks::install() {
	INSTALL(swapWindow);
	INSTALL(pollEvent);
	INSTALL(sdlDelay);
	INSTALL(mouseRelativeUpdate);
	INSTALL(renderFrame);
	INSTALL(drawHud);
	// INSTALL(drawText);
	INSTALL(drawMainMenu);
	INSTALL(drawCreditsMenu);
	INSTALL(drawOptionsMenu);
	INSTALL(drawMenuButton);
	INSTALL(serverEventLoop);
	// INSTALL(testHook);
	// INSTALL(renderPNG);

	spdlog::info("Hooks installed!");
}
