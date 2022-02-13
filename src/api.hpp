#pragma once

#include <string_view>

#include "imconfig.h"
#include "imgui.h"

namespace api {
extern ImDrawList* bgDrawList;
void setDrawList(ImDrawList* n);
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a);
void drawTextImGui(std::string_view text, float x, float y, float scale,
                   int params, float r, float g, float b, float a);
void drawRectFilledImGui(ImVec2 start, ImVec2 dimensions, ImColor col,
                         float rounding, ImDrawFlags flags);

};  // namespace api