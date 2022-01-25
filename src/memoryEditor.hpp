#pragma once

#include "gui.hpp"
#include "imgui_memory_editor.hpp"

class memoryEditor : public gui {
 public:
	using gui::gui;

	void onResize(ImVec2 s) override {
	};

	void handleKeyPress(SDL_Event* event) override {
	};

	void preBegin() { }

	void postBegin() { }

	void drawContents() {
		if (!mem) mem = std::make_unique<MemoryEditor>();
		mem->DrawContents(memData, memSize);
	}

    void* memData;
    int memSize;
 private:
	std::unique_ptr<MemoryEditor> mem;
};

inline std::unique_ptr<memoryEditor> g_memoryEditor;