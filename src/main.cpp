#include <fstream>
#include <memory>
#include <system_error>

#ifdef _WIN32
#include <WinSock2.h>  // always needs to be included before "Windows.h"
#include <Windows.h>
#endif

#include "game.hpp"
#include "hooks.hpp"
#include "structs.hpp"
#include "tcpSocket.hpp"
#include "utils.hpp"

#ifdef _WIN32
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDll, _In_ DWORD fdwReason,
                    _In_opt_ LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (!AllocConsole()) {
			ERROR_AND_EXIT("Failed to allocate console.");
			return false;
		}

		_iobuf* data;
		const errno_t res = freopen_s(&data, "CONOUT$", "w", stdout);
		if (res != 0) {
			ERROR_AND_EXIT(
			    fmt::format("Failed to open stdout filestream. Error code: %i (%s)",
			                res, std::system_category().message(res)));
			return false;
		}

		if (!SetConsoleTitleA("Sub Rosa: Custom Console")) {
			ERROR_AND_EXIT("Failed to set console title.");
			return false;
		}

		WSAData wsaData;

		if (int result = WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			throw std::runtime_error("Failed to intalize WSA");
		}
#else

void __attribute__((constructor)) entry() {
#endif
		g_utils = std::make_unique<utils>(INFO);
		g_game = std::make_unique<game>();
		g_utils->log(INFO,
		             fmt::format("Base address: {:#x}", g_game->getBaseAddress()));
		g_hooks = std::make_unique<hooks>();
		g_hooks->install();
		g_utils->log(INFO, "Everything done");

#ifdef _WIN32
	}
	return TRUE;
#endif
}