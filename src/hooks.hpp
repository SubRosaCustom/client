#pragma once

#include <memory>
#include <type_traits>
#include "subhook.h"

class hooks
{
public:
    hooks();
    subhook::Hook renderFrameHook;
};

inline std::unique_ptr<hooks> g_hooks;