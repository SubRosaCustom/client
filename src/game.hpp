#pragma once

#include <memory>
#include <cstdint>

class game
{
public:
    game(std::uintptr_t base);

    std::uintptr_t drawTextShadow;
    std::add_pointer_t<int64_t(char *, int32_t, char, float, float, float, float, float, float, float)> drawTextShadowFunc;

    std::uintptr_t renderFrame;
    std::add_pointer_t<int64_t(int64_t, int64_t, double*)> renderFrameFunc;
    
    std::uintptr_t drawHud;
    std::add_pointer_t<int64_t(int64_t)> drawHudFunc;
};

inline std::unique_ptr<game> g_game;