#include "game.hpp"
#include "structs.hpp"

game::game(std::uintptr_t base) {
    drawText = base + WIN_LIN(0x6d930, 0x2c62c);
    drawTextFunc = (decltype(drawTextFunc))(drawText);
    
    renderFrame = base + WIN_LIN(0x11fd00, 0x127844);
    renderFrameFunc = (decltype(renderFrameFunc))(renderFrame);

    drawHud = base + WIN_LIN(0x86000, 0x5bfed);
    drawHudFunc = (decltype(drawHudFunc))(drawHud);

    drawMainMenu = base + WIN_LIN(0xfe5d0, 0x8d9c2);
    drawMainMenuFunc = (decltype(drawMainMenuFunc))(drawMainMenu);

    drawCreditsMenu = base + WIN_LIN(0xfcc90, 0x8d1ed);
    drawCreditsMenuFunc = (decltype(drawCreditsMenuFunc))(drawCreditsMenu);
}