#pragma once

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

	std::add_pointer_t<void(void)> voidFunc;

	std::uintptr_t drawText;
#ifdef _WIN32
	std::add_pointer_t<int64_t(char *, float, float, float, int, float, float,
	                           float, float, ...)>
	    drawTextFunc;
#else
	std::add_pointer_t<int64_t(char *, int, int, int, float, float, float, float,
	                           float, float, float, void *)>
	    drawTextFunc;
#endif

	std::uintptr_t renderFrame;
	std::add_pointer_t<int64_t(int64_t, int64_t, double *)> renderFrameFunc;

	std::uintptr_t drawHud;
	std::add_pointer_t<int64_t(int64_t)> drawHudFunc;

	std::uintptr_t drawMainMenu;
	std::add_pointer_t<int(void)> drawMainMenuFunc;

	std::uintptr_t drawCreditsMenu;
	std::add_pointer_t<int(void)> drawCreditsMenuFunc;

	std::uintptr_t createSound;
	std::add_pointer_t<int(float, float, int, Vector3 *)> createSoundFunc;

	std::uintptr_t createParticle;
	std::add_pointer_t<int(float, int, Vector3 *, Vector3 *)> createParticleFunc;
};

inline std::unique_ptr<game> g_game;