#pragma once

#ifdef _WIN32
#define WIN_LIN(win, lin) win
#define RETURN_ADDRESS() std::uintptr_t(_ReturnAddress())
#define FRAME_ADDRESS() (std::uintptr_t(_AddressOfReturnAddress()) - sizeof(std::uintptr_t))
#else
#define WIN_LIN(win, lin) lin
#define RETURN_ADDRESS() std::uintptr_t(__builtin_return_address(0))
#define FRAME_ADDRESS() std::uintptr_t(__builtin_frame_address(0))
#endif

struct Vector3
{
    float x, y, z;

    Vector3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    };
};