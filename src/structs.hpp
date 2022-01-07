#pragma once

#include <errno.h>

#ifdef _WIN32
#define WIN_LIN(win, lin) win
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() \
	(std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#define ERROR_AND_EXIT(error)                                                                                                                    \
	{                                                                                                                                              \
		MessageBoxA(NULL, fmt::format("{}, Error no: {} ({})", error, errno, std::system_category().message(errno)), "Client", MB_OK | MB_ICONERROR); \
		exit(1);                                                                                                                                     \
	}

#define THISCALL __thiscall
#define FASTCALL __fastcall
#define CDECL __cdecl
#define STDCALL __stdcall

#else
#define WIN_LIN(win, lin) lin
#define RETURN_ADDRESS() std::uintptr_t(__builtin_return_address(0))
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))
#define ERROR_AND_EXIT(error)                                                                                    \
	g_utils->log(ERROR, fmt::format("{}, Error no: {} ({})", error, errno, std::system_category().message(errno))); \
	exit(1);

#define THISCALL
#define FASTCALL
#define CDECL
#define STDCALL

#endif

struct Vector3 {
	float x, y, z;

	Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	};
};