#include <memory>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "game.hpp"
#include "hooks.hpp"

uintptr_t baseAddress;

#ifdef _WIN32

inline uintptr_t getBaseAddress() {
    return 0;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    if (reason == DLL_PROCESS_ATTACH) {
        // for windows 0x6d930
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