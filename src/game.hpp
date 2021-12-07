#pragma once

#include <memory>
#include <cstdint>

#define TEXT_CENTER 0x1
#define TEXT_SHADOW 0x20

class game
{
public:
    game(std::uintptr_t base);

    std::uintptr_t drawText;
    std::add_pointer_t<int64_t(char *, int, int, int, float, float, float, float, float, float, float)> drawTextFunc;

    std::uintptr_t renderFrame;
    std::add_pointer_t<int64_t(int64_t, int64_t, double*)> renderFrameFunc;
    
    std::uintptr_t drawHud;
    std::add_pointer_t<int64_t(int64_t)> drawHudFunc;
    
    std::uintptr_t drawMainMenu;
    std::add_pointer_t<int64_t(void)> drawMainMenuFunc;
    
    std::uintptr_t drawCreditsMenu;
    std::add_pointer_t<int64_t(void)> drawCreditsMenuFunc;
};

inline std::unique_ptr<game> g_game;