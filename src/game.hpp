#pragma once

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <fcntl.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <SDL2/SDL.h>

#include <cstdint>
#include <fstream>
#include <memory>

#include "structs.hpp"

#define TEXT_CENTER 0x1
#define TEXT_UP 0x4
#define TEXT_DOWN 0x8
#define TEXT_LEFT 0x2
#define TEXT_RIGHT 0x0
#define TEXT_SHADOW 0x20
#define TEXT_FORMAT 0x40

#include "structs.hpp"

class game {
 private:
	std::uintptr_t baseAddress;

 public:
	game();
	inline uintptr_t getBaseAddress() {
		if (!baseAddress) {
#if _WIN32
			baseAddress = (uintptr_t)GetModuleHandle(NULL);
#else
			std::ifstream f("/proc/self/maps");
			std::string ln;
			std::getline(f, ln);
			auto addrStr = ln.substr(0, ln.find("-"));

			baseAddress = std::stoul(addrStr, nullptr, 16);
#endif
		}
		return baseAddress;
	}

	ServerListEntry* serverListEntries;
	int* amountOfServerListEntries;
	Mouse* mouse;
	ChatMessage* chatMessages;
	int* amountOfChatMessages;
	int* numEventsNeedSync;
	int* isInGame;
	int* mainMenuState;
	int* escapeMenuState;
	int* csKeyboardIndex;
	CSKeyboard* csKeyboard;

	std::uintptr_t swapWindow;
	std::add_pointer_t<decltype(SDL_GL_SwapWindow)> swapWindowFunc;

	std::uintptr_t pollEvent;
	std::add_pointer_t<decltype(SDL_PollEvent)> pollEventFunc;

	std::uintptr_t sdlDelay;
	std::add_pointer_t<decltype(SDL_Delay)> sdlDelayFunc;

	std::add_pointer_t<void(void)> voidFunc;

	std::uintptr_t drawText;
#ifdef _WIN32
	std::add_pointer_t<int64_t(char*, float, float, float, int, float, float,
	                           float, float, ...)>
	    drawTextFunc;
#else
	std::add_pointer_t<int64_t(char*, int, int, int, float, float, float, float,
	                           float, float, float, void*)>
	    drawTextFunc;
#endif

	std::uintptr_t mouseRelativeUpdate;
	std::add_pointer_t<void(Mouse*)> mouseRelativeUpdateFunc;

	std::uintptr_t renderFrame;
	std::add_pointer_t<int64_t(int64_t, int64_t, double*)> renderFrameFunc;

	std::uintptr_t drawHud;
	std::add_pointer_t<int64_t(int64_t)> drawHudFunc;

	std::uintptr_t drawMainMenu;
	std::add_pointer_t<int(void)> drawMainMenuFunc;

	std::uintptr_t drawCreditsMenu;
	std::add_pointer_t<int(void)> drawCreditsMenuFunc;

	std::uintptr_t drawOptionsMenu;
	std::add_pointer_t<int(void)> drawOptionsMenuFunc;

	std::uintptr_t createSound;
	std::add_pointer_t<int(float, float, int, Vector3*)> createSoundFunc;

	std::uintptr_t createParticle;
	std::add_pointer_t<int(float, int, Vector3*, Vector3*)> createParticleFunc;

	std::uintptr_t createNewspaperText;
	std::add_pointer_t<int(int, int)> createNewspaperTextFunc;

	std::uintptr_t createStreetSignText;
	std::add_pointer_t<int(int, int)> createStreetSignTextFunc;

	std::uintptr_t chatAddMessage;
	std::add_pointer_t<int(int, char*, int, int)> chatAddMessageFunc;

	std::uintptr_t drawMenuCheckbox;
	std::add_pointer_t<int(char*, int*)> drawMenuCheckboxFunc;

	std::uintptr_t drawMenuText;
	std::add_pointer_t<void(char*)> drawMenuTextFunc;

	std::uintptr_t drawMenuButton;
	std::add_pointer_t<int(char*)> drawMenuButtonFunc;
	
	std::uintptr_t serverEventLoop;
	std::add_pointer_t<void(void)> serverEventLoopFunc;

	std::uintptr_t testHook;
	std::add_pointer_t<int(void)> testHookFunc;
	
	std::uintptr_t renderPNG;
	std::add_pointer_t<int(int, int, char, float, float, float, float)>
	    renderPNGFunc;
	
	std::uintptr_t createTexture;
	std::add_pointer_t<void(int, int, int, int, int, int)> createTextureFunc;
	
	std::uintptr_t loadCMO;
	std::add_pointer_t<int(int, int, char*)> loadCMOFunc;
	
	std::uintptr_t loadCMC;
	std::add_pointer_t<void(int, char*)> loadCMCFunc;
};

inline std::unique_ptr<game> g_game;