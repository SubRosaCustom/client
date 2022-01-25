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
void init() { bgDrawList = ImGui::GetBackgroundDrawList(); }
void setDrawList(ImDrawList* n) { bgDrawList = n; }
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
		ImGuiContext& ctx = *ImGui::GetCurrentContext();

		bgDrawList->AddText(ctx.Font, scale, ImVec2(x + 1, y + 1),
		                    ImColor(0.f, 0.f, 0.f, a), text.data());
		bgDrawList->AddText(ctx.Font, scale, ImVec2(x, y), ImColor(r, g, b, a),
		                    text.data());
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
}  // namespace api