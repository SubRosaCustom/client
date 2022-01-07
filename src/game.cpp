#include "game.hpp"

#include "structs.hpp"

game::game(std::uintptr_t base) {
	drawText = base + WIN_LIN(0x6d930, 0x2c62c);
	drawTextFunc = (decltype(drawTextFunc))(drawText);

	renderFrame = base + WIN_LIN(0x11fd00, 0x127844);
	renderFrameFunc = (decltype(renderFrameFunc))(renderFrame);

	drawHud = base + WIN_LIN(0x86000, 0x5bfed);
	drawHudFunc = (decltype(drawHudFunc))(drawHud);

	drawMainMenu = base + WIN_LIN(0xfe1b0, 0x8d9c2);
	drawMainMenuFunc = (decltype(drawMainMenuFunc))(drawMainMenu);

	drawCreditsMenu = base + WIN_LIN(0xfcc90, 0x8d1ed);
	drawCreditsMenuFunc = (decltype(drawCreditsMenuFunc))(drawCreditsMenu);

	// win: fd300, lin: 13b85b join menu void(void)
	// win: fc480, lin: 14f048 password menu void(void)
	// win: feb50, lin: af684 options menu void(void)
	//
	// win: 8f1b0, lin: 11e57d handle events
	// win: 473de460, lin: 1a80f7a0 events struct
	// win: 49c1dc60, lin: e6c640 current event num
	// win: 49c1dc64, lin: 56e87f40 amount of events

	createSound = base + WIN_LIN(0x5ed40, 0xe4151);
	createSoundFunc = (decltype(createSoundFunc))(createSound);

	createParticle = base + WIN_LIN(0x5ee40, 0xd957e);
	createParticleFunc = (decltype(createParticleFunc))(createParticle);
}