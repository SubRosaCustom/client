#include "api.hpp"

#include <cstdarg>
#include <string>

#include "game.hpp"
#include "hooks.hpp"
#include "imgui_internal.h"

#define REMOVE_HOOK(name) \
	subhook::ScopedHookRemove name##Remove(&g_hooks->name##Hook);

namespace api {
ImDrawList* bgDrawList;
std::vector<std::unique_ptr<APIMessage_t>> messages = {};

void frame(ImDrawList* n) {
	bgDrawList = n;
	messages.erase(std::remove_if(messages.begin(), messages.end(),
	                              [](auto &&i) { return i->countdownStart <= 0; }),
	               messages.end());

	for (auto &&i : messages) {
		i->countdownStart = i->countdownStart - 1;
		spdlog::info("ben: {}", i->text);
		drawTextImGui(i->text, i->x, i->y, i->scale, 0, i->r, i->g, i->b,
		              i->countdownStart / 200.f);
	}
}
void drawText(std::string_view text, float x, float y, float scale, int params,
              float r, float g, float b, float a) {
#ifdef _WIN32
	g_game->drawTextFunc((char*)text.data(), x, y, scale, params, r, g, b, a);
#else
	g_game->drawTextFunc((char*)text.data(), params, 0, 0, x, y, scale, r, g, b,
	                     a, 0);
#endif
}
void drawTextImGui(std::string_view text, float x, float y, float scale,
                   int params, float r, float g, float b, float a) {
	if (bgDrawList) {
		auto ctx = ImGui::GetIO();

		bgDrawList->AddText(ctx.FontDefault, scale, ImVec2(x + 1, y + 1),
		                    ImColor(0.f, 0.f, 0.f, a), text.data());
		bgDrawList->AddText(ctx.FontDefault, scale, ImVec2(x, y),
		                    ImColor(r, g, b, a), text.data());
	}
}
void drawRectFilledImGui(ImVec2 start, ImVec2 dimensions, ImColor col,
                         float rounding, ImDrawFlags flags) {
	if (bgDrawList) {
		ImGuiContext& ctx = *ImGui::GetCurrentContext();

		bgDrawList->AddRectFilled(
		    ImVec2(start.x, start.y),
		    ImVec2(start.x + dimensions.x, start.y + dimensions.y), col, rounding,
		    flags);
	}
}
void addText(std::string text, float x, float y, float scale,
             int countdownStart, float r, float g, float b) {
	
	messages.push_back(std::make_unique<APIMessage_t>(text, x, y, scale, countdownStart, r, g, b));
}
}  // namespace api