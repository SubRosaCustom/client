#include "game.hpp"

#include "structs.hpp"
#include "utils/utils.hpp"

game::game() {
	auto base = getBaseAddress();

#if _WIN32
#else
	const auto libSDL = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_NOLOAD);

	pollEvent = uintptr_t(dlsym(libSDL, "SDL_PollEvent"));
	pollEventFunc = (decltype(pollEventFunc))(pollEvent);

	swapWindow = uintptr_t(dlsym(libSDL, "SDL_GL_SwapWindow"));
	swapWindowFunc = (decltype(swapWindowFunc))(swapWindow);

	sdlDelay = uintptr_t(dlsym(libSDL, "SDL_Delay"));
	sdlDelayFunc = (decltype(sdlDelayFunc))(sdlDelay);

	dlclose(libSDL);
#endif

	serverListEntries = (ServerListEntry*)(base + WIN_LIN(0x2b0a7f58, 0x1c784de0));
	amountOfServerListEntries = (int*)(base + WIN_LIN(0x2b0a7ee4, 0x0a8e2460));
	mouse = (Mouse*)(base + WIN_LIN(0x43f7c6c0, 0x38cced60));
	chatMessages = (ChatMessage*)(base + WIN_LIN(0x52e8f660, 0x5e15da60));
	amountOfChatMessages = (int*)(base + WIN_LIN(0x52e98e60, 0x1057620));
	numEventsNeedSync = (int*)(base + WIN_LIN(0x64d2dd08, 0x1820cfe8));

	// This function is inlined in Windows
	// I LOVE MSVC
#ifndef _WIN32
	mouseRelativeUpdate = base + WIN_LIN(TODO, 0x2ae9d);
	mouseRelativeUpdateFunc =
	    (decltype(mouseRelativeUpdateFunc))(mouseRelativeUpdate);
#endif

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
	
	drawOptionsMenu = base + WIN_LIN(0xfeb50, 0xaf684);
	drawOptionsMenuFunc = (decltype(drawOptionsMenuFunc))(drawOptionsMenu);

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

	chatAddMessage = base + WIN_LIN(0x588f0, 0x4ee95);
	chatAddMessageFunc = (decltype(chatAddMessageFunc))(chatAddMessage);

	drawMenuCheckbox = base + WIN_LIN(0x71910, 0x26168);
	drawMenuCheckboxFunc = (decltype(drawMenuCheckboxFunc))(drawMenuCheckbox);

	drawMenuText = base + WIN_LIN(0x71640, 0x2e21e);
	drawMenuTextFunc = (decltype(drawMenuTextFunc))(drawMenuText);

	drawMenuButton = base + WIN_LIN(TODO, 0x2602f);
	drawMenuButtonFunc = (decltype(drawMenuButtonFunc))(drawMenuButton);

	serverEventLoop = base + WIN_LIN(TODO, 0x14067f);
	serverEventLoopFunc = (decltype(serverEventLoopFunc))(serverEventLoop);

	renderPNG = base + WIN_LIN(0x6d300, 0x2f166);
	renderPNGFunc = (decltype(renderPNGFunc))(renderPNG);
}