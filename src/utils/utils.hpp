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

#include "spdlog/formatter.h"
#include "spdlog/spdlog.h"

static inline constexpr int subRosaCustomMagic = 0x53524345;  // hex for "SRCE"
static inline constexpr std::array<int, 3> customVersion = {
    0, 0, 1};  // semantic versioning

#ifdef _WIN32
#undef ERROR  // awful MSVC fix

#define WIN_LIN(win, lin) win
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() \
	(std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#define ERROR_AND_EXIT(error)                                                  \
	{                                                                            \
		MessageBoxA(NULL,                                                          \
		            spdlog::fmt_lib::format("{}, Error no: {} ({})", error, errno, \
		                                    std::system_category().message(errno)) \
		                .c_str(),                                                  \
		            "Client", MB_OK | MB_ICONERROR);                               \
		exit(1);                                                                   \
	}

#define THISCALL __thiscall
#define FASTCALL __fastcall
#define CDECL __cdecl
#define STDCALL __stdcall

#else
#define WIN_LIN(win, lin) lin
#define RETURN_ADDRESS() std::uintptr_t(__builtin_return_address(0))
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))
#define ERROR_AND_EXIT(text)                            \
	spdlog::error("{}, Error no: {} ({})", text, errno,   \
	              std::system_category().message(errno)); \
	exit(1);

#define THISCALL
#define FASTCALL
#define CDECL
#define STDCALL

#endif

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)

#define PAD(size) char PAD_NAME(__LINE__)[size];

class utils {
 public:
	utils(spdlog::level::level_enum level) { spdlog::set_level(level); };
};

inline std::unique_ptr<utils> g_utils;

#ifdef _WIN32
#define ERROR 0  // awful MSVC fix
#endif