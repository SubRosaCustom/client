#pragma once

#include <errno.h>

#ifdef _WIN32
#define WIN_LIN(win, lin) win
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() \
	(std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#define ERROR_AND_EXIT(error)                                       \
	{                                                                 \
		MessageBoxA(NULL,                                               \
		            fmt::format("{}, Error no: {} ({})", error, errno,  \
		                        std::system_category().message(errno)), \
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

struct Vector3 {
	float x, y, z;

	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	};
};

static constexpr int maxServerListEntries = 32;
// at 0x6a8a76e8 in linux
// 0x5c bytes
struct ServerListEntry {
	int versionMajor;
	int versionMinor; // 4
	int networkVersion; // 8
	char name[32]; // c
	PAD(0x40 - 0xc - 32);
	int numOfPlayers; // 40
	int unk0; // 44
	int maxNumOfPlayers;  // 48
	int ping; // 4c
	int gameType; // 50
	PAD(0x5c - 0x50 - 4);
};