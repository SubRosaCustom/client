#include <memory>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "structs.hpp"
#include "game.hpp"
#include "hooks.hpp"

uintptr_t baseAddress;

#ifdef _WIN32

inline uintptr_t getBaseAddress() {
    return (uintptr_t)GetModuleHandle(NULL);
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        if (!AllocConsole()) {
            _RPTF1(_CRT_ERROR, "Failed to allocate console. Error code: %i", GetLastError());
            return false;
        }

        _iobuf* data;
        const errno_t res = freopen_s(&data, "CONOUT$", "w", stdout);
        if (res != 0) {
            _RPTF1(_CRT_ERROR, "Failed to open stdout filestream. Error code: %i", res);
            return false;
        }

        if (!SetConsoleTitleA("BEAAAAAAAAAAAAAAAAANS")) {
            _RPTF1(_CRT_WARN, "Failed to set console title. Error code: %i", GetLastError());
            return false;
        }

        g_game = std::make_unique<game>(getBaseAddress());
        g_hooks = std::make_unique<hooks>();
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

void __attribute__((constructor)) entry()
{
    g_game = std::make_unique<game>(getBaseAddress());
    g_hooks = std::make_unique<hooks>();
}

#endif