#pragma once

#include <string_view>
#include <string>
#include <memory>
#include <vector>

#include "imconfig.h"
#include "imgui.h"

struct APIMessage_t {
	std::string text;
	float x;
	float y;
	float scale;
	int countdownStart;
	float r;
	float g;
	float b;
};

namespace api {
extern ImDrawList* bgDrawList;
extern std::vector<std::unique_ptr<APIMessage_t>> messages;
void frame(ImDrawList* n);
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a);
void drawTextImGui(std::string_view text, float x, float y, float scale,
                   int params, float r, float g, float b, float a);
void drawRectFilledImGui(ImVec2 start, ImVec2 dimensions, ImColor col,
                         float rounding, ImDrawFlags flags);
void addText(std::string text, float x, float y, float scale,
             int countdownStart, float r, float g, float b);

};  // namespace api