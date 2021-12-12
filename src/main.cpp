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
#if 1
        if (!AllocConsole()) {
            ERROR_AND_EXIT("Failed to allocate console. Error code: %i", GetLastError());
            return false;
        }

        _iobuf* data;
        const errno_t res = freopen_s(&data, "CONOUT$", "w", stdout);
        if (res != 0) {
            ERROR_AND_EXIT("Failed to open stdout filestream. Error code: %i", res);
            return false;
        }

        if (!SetConsoleTitleA("Sub Rosa: Custom Console")) {
            ERROR_AND_EXIT("Failed to set console title. Error code: %i", GetLastError());
            return false;
        }
#endif

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