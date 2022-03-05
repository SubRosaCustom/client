#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../api.hpp"
#include "../../imgui/imgui.h"

struct notification {
	int countdown;
	std::string message;
};

class notificationManager {
 private:
	ImVec2 startPos;
	ImVec2 padding;
	std::vector<std::unique_ptr<notification>> notficiations;

 public:
	notificationManager(ImVec2 _startPos = ImVec2(0, 0),
	                    ImVec2 _padding = ImVec2(20, 20))
	    : startPos(_startPos), padding(_padding){};
	~notificationManager() {};

	void addNotification(std::string _message, int countdownStart) {
		notficiations.push_back(
		    std::make_unique<notification>(countdownStart, _message));
	}

	void render() {
		int counter = 0;
		float y = ImGui::CalcTextSize("a").y;
		for (auto &&notif : notficiations) {
			api::drawRectFilledImGui(
			    ImVec2(startPos.x, (padding.y + y) * counter),
			    ImVec2(padding.x + ImGui::CalcTextSize(notif->message.c_str()).x,
			           padding.y + y),
			    ImColor(0.2f, 0.2f, 0.2f, 0.2f), 2.f, ImDrawFlags_RoundCornersAll);
                api::drawTextImGui(notif->message, startPos.x + padding.x, (padding.y + y) * counter, 13.f, 0, 1.f, 1.f, 1.f, 1.f);
			counter++;
		}
	}
};

inline std::unique_ptr<notificationManager> g_notificationManager;