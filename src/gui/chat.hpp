#pragma once

#include "../utils/settings.hpp"
#include "gui.hpp"

class chatWindow : public gui {
 public:
	using gui::gui;

	void onResize(ImVec2 s) override{};

	void handleKeyPress(SDL_Event* event) override{};

	void preBegin() {
		ImGui::PushStyleColor(ImGuiCol_WindowBg,
		                      ImVec4(backgroundColor[0], backgroundColor[1],
		                             backgroundColor[2], backgroundColor[3]));

		if (*g_game->escapeMenuState == 0 && *g_game->isInGame == 1)
			windowFlags |= (ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
		else
            windowFlags &= ~(ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    }

	void postBegin() {
		ImGui::PopStyleColor();
	}

	void drawContents() {
		if (!g_settings->get_var<bool>("small_chat")) {
			isOpen = false;
			return;
		}

		static float decayRatio = 200.f;
		static float fontScale = ImGui::GetIO().FontDefault->Scale * 100.f;

		if (ImGui::BeginPopupContextItem("Settings##chatWindow")) {
			ImGui::SliderFloat("Decaying Message Alpha", &decayRatio, 100.0f, 900.0f,
			                   "%.3f");
			if (ImGui::SliderFloat("Font Size", &fontScale, 0.0f, 100.0f,
			                   "%.3f"))
							   ImGui::SetWindowFontScale(fontScale / 100.f);

			ImGui::ColorEdit4("Background Color", backgroundColor);

			ImGui::EndPopup();
		}

		for (size_t i = 0; i <= *g_game->amountOfChatMessages; i++) {
			auto message = g_game->chatMessages[i];
			if (message.timer > 0) {
				auto gb = 1.f;
				if (message.type == 6) gb = 0.f;

				ImGui::PushStyleColor(ImGuiCol_Text,
				                      ImVec4(1.f, gb, gb, message.timer / decayRatio));
				ImGui::TextWrapped(message.message);
				ImGui::PopStyleColor();
			}
		}
	}

	float backgroundColor[4] = {0.2f, 0.2f, 0.2f, 0.6f};
	void* memData;
	int memSize;

 private:
};

inline std::unique_ptr<chatWindow> g_chatWindow;