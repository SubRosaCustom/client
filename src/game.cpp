#include "game.hpp"

game::game(std::uintptr_t base) {
    drawTextShadow = base + 0x2c62c;
    drawTextShadowFunc = (decltype(drawTextShadowFunc))(drawTextShadow);
    
    renderFrame = base + 0x127844;
    renderFrameFunc = (decltype(renderFrameFunc))(renderFrame);
}