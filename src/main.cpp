#include <fstream>
#include <memory>

#ifdef _WIN32
#include <WinSock2.h>  // always needs to be included before "Windows.h"
#include <Windows.h>
#endif

#include "game.hpp"
#include "hooks.hpp"
#include "structs.hpp"
#include "tcpSocket.hpp"
#include "utils.hpp"

uintptr_t baseAddress;

#ifdef _WIN32

inline uintptr_t getBaseAddress() { return (uintptr_t)GetModuleHandle(NULL); }

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDll, _In_ DWORD fdwReason,
                    _In_opt_ LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (!AllocConsole()) {
			ERROR_AND_EXIT("Failed to allocate console. Error code: %i",
			               GetLastError());
			return false;
		}

		_iobuf* data;
		const errno_t res = freopen_s(&data, "CONOUT$", "w", stdout);
		if (res != 0) {
			ERROR_AND_EXIT("Failed to open stdout filestream. Error code: %i", res);
			return false;
		}

		if (!SetConsoleTitleA("Sub Rosa: Custom Console")) {
			ERROR_AND_EXIT("Failed to set console title. Error code: %i",
			               GetLastError());
			return false;
		}

		WSAData wsaData;

		if (int result = WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			throw std::runtime_error("Failed to intalize WSA");
		}

		g_utils = std::make_unique<utils>(INFO);
		g_game = std::make_unique<game>(getBaseAddress());
		g_hooks = std::make_unique<hooks>();
		g_hooks->install();
	}
	return TRUE;
}

#else

inline uintptr_t getBaseAddress() {
	if (!baseAddress) {
		std::ifstream f("/proc/self/maps");
		std::string ln;
		std::getline(f, ln);
		auto addrStr = ln.substr(0, ln.find("-"));

		baseAddress = std::stoul(addrStr, nullptr, 16);
	}
	return baseAddress;
}

void __attribute__((constructor)) entry() {
	g_utils = std::make_unique<utils>(INFO);
	g_game = std::make_unique<game>(getBaseAddress());
	g_hooks = std::make_unique<hooks>();
	g_utils->log(DEBUG, "Fortnite Card");
}

#endif