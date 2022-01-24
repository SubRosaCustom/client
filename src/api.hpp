#pragma once

#include <string_view>

#include "imconfig.h"
#include "imgui.h"

namespace api {
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a);

};