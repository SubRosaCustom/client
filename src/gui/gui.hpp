#pragma once

#include <string_view>
#include <vector>

#include "imconfig.h"
#include "imgui.h"

class gui;
extern std::vector<gui *> activeGuiList;

typedef int GuiFlags;
enum GuiFlags_ {
	GuiFlags_None = 0,
	GuiFlags_OpenOnStart = 1 << 0,
	GuiFlags_DisablesMouse = 1 << 1,
};

class gui {
 public:
	explicit gui(std::string_view title = "Default GUI Title",
	             GuiFlags _guiFlags = GuiFlags_None,
	             ImVec2 startPos = ImVec2(300, 200),
	             ImVec2 startSize = ImVec2(400, 300),
	             ImGuiWindowFlags flags = ImGuiWindowFlags_None)
	    : title(title),
	      isOpen(_guiFlags & GuiFlags_OpenOnStart),
	      pos(startPos),
	      size(startSize),
	      windowFlags(flags),
	      disableMouse(_guiFlags & GuiFlags_DisablesMouse) {
		activeGuiList.push_back(this);
	};
	explicit gui(gui &&) = default;
	explicit gui(const gui &) = default;
	gui &operator=(gui &&) = default;
	gui &operator=(const gui &) = default;
	~gui(){};

	virtual void drawContents() = 0;

	virtual void onResize(ImVec2 s) { size = s; }
	virtual void handleKeyPress(SDL_Event *event) {}

	virtual void preBegin() = 0;
	virtual void postBegin() = 0;

	void draw() {
		if (isOpen) {
			if (setFocus) {
				setFocus = false;
				ImGui::SetNextWindowFocus();
			}

			preBegin();
			if (ImGui::Begin(title.data(), &isOpen, windowFlags)) {
				ImGui::SetWindowSize(size, ImGuiCond_Once);
				if (prevSize.x && prevSize.x != size.x && prevSize.y != size.y) {
					ImGui::SetWindowSize(size, ImGuiCond_Always);
				}
				ImGui::SetWindowPos(pos, ImGuiCond_Once);
				drawContents();
			}
			ImGui::End();
			postBegin();
		}

		prevSize = size;
	};

	bool disableMouse;
	bool isOpen;
	bool setFocus;
	std::string_view title;
	ImVec2 pos;
	ImVec2 size;
	ImVec2 prevSize;
	ImGuiWindowFlags windowFlags;

 private:
};