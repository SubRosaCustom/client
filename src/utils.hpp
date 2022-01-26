#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "fmt/chrono.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/format.h"

static inline constexpr int subRosaCustomPortStart = 26000;
static inline constexpr int subRosaCustomMagic = 0x53524345; // hex for "SRCE"
static inline constexpr std::array<int, 3> customVersion = {0, 0, 1}; // semantic versioning

#ifdef _WIN32
#undef ERROR  // awful MSVC fix

#define WIN_LIN(win, lin) win
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() \
	(std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#define ERROR_AND_EXIT(error)                                       \
	{                                                                 \
		MessageBoxA(NULL,                                               \
		            fmt::format("{}, Error no: {} ({})", error, errno,  \
		                        std::system_category().message(errno)) \
		                .c_str(), \
		            "Client", MB_OK | MB_ICONERROR);                    \
		exit(1);                                                        \
	}

#define THISCALL __thiscall
#define FASTCALL __fastcall
#define CDECL __cdecl
#define STDCALL __stdcall

#else
#define WIN_LIN(win, lin) lin
#define RETURN_ADDRESS() std::uintptr_t(__builtin_return_address(0))
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))
#define ERROR_AND_EXIT(error)                                              \
	g_utils->log(ERROR, fmt::format("{}, Error no: {} ({})", error, errno,   \
	                                std::system_category().message(errno))); \
	exit(1);

#define THISCALL
#define FASTCALL
#define CDECL
#define STDCALL

#endif

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)

#define PAD(size) char PAD_NAME(__LINE__)[size];

enum LOG_LEVELS {
	DEBUG,
	INFO,
	WARN,
	ERROR,
};

class utils {
 public:
	int minLogLevel;

	utils(LOG_LEVELS level) : minLogLevel(level){};

	void printLogPrefix(LOG_LEVELS level) {
		switch (level) {
			case DEBUG:
				fmt::print(fg(fmt::color::blue), "Debug");
				break;
			case INFO:
				fmt::print(fg(fmt::color::green), "Info");
				break;
			case WARN:
				fmt::print(fg(fmt::color::yellow), "Warn");
				break;
			case ERROR:
				fmt::print(fg(fmt::color::red), "Error");
				break;
			default:
				fmt::print(fg(fmt::color::red), "Unk");
				break;
		}
	}

	void log(LOG_LEVELS type, std::string text) {
		if (minLogLevel <= (int)type) {
			time_t timetoday;
			time(&timetoday);
			fmt::print(fg(fmt::color::gray), "[");
			fmt::print("{:%Y-%m-%d %X} ", std::chrono::system_clock::now());
			printLogPrefix(type);
			fmt::print(fg(fmt::color::gray), "]");
			fmt::print(" {}\n", text);
		}
	};
};

inline std::unique_ptr<utils> g_utils;

#ifdef _WIN32
#define ERROR 0  // awful MSVC fix
#endif