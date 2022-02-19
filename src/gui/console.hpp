#pragma once

#include <array>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "../utils/utils.hpp"
#include "gui.hpp"
#include "imterm/terminal.hpp"
#include "imterm/terminal_helpers.hpp"
#include "memoryEditor.hpp"

struct custom_command_struct {
	bool should_close = false;
};

class terminal_commands
    : public ImTerm::basic_spdlog_terminal_helper<
          terminal_commands, custom_command_struct, misc::no_mutex> {
 public:
	terminal_commands();

	static std::vector<std::string> no_completion(argument_type&) { return {}; }

	static void clear(argument_type&);
	static void configure_term(argument_type&);
	static std::vector<std::string> configure_term_autocomplete(argument_type&);
	static void echo(argument_type&);
	static void exit(argument_type&);
	static void help(argument_type&);
	static void quit(argument_type&);
};

class console : public gui {
 public:
	using gui::gui;
	void init() {
		cmd_struct = std::make_unique<custom_command_struct>();  // terminal commands can interact with this structure
		terminal_log = std::make_unique<ImTerm::terminal<terminal_commands>>(*cmd_struct);
		terminal_log->set_min_log_level(ImTerm::message::severity::info);
	};

	void onResize(ImVec2 s) override {
		this->size = s;
		this->size.y = this->size.y / 3;
	};

	void handleKeyPress(SDL_Event* event) override {
		if (ImGui::IsKeyPressed(ImGuiKey_Insert)) {
			this->isOpen = !this->isOpen;
			if (this->isOpen) this->setFocus = true;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_F) &&
		    (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
		     ImGui::IsKeyDown(ImGuiKey_RightCtrl))) {
			// con->ShowFilters = !con->ShowFilters;
		}
	};

	void preBegin() { ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f); }

	void postBegin() { ImGui::PopStyleVar(); }

	void drawContents() {
		// // ImGui::ShowDemoWindow();
		// if (!con) con = std::make_unique<AppConsole>();
		// con->Draw("Console", (bool*)true);
	}

 private:
	// std::unique_ptr<AppConsole> con;
	std::unique_ptr<custom_command_struct> cmd_struct;
	std::unique_ptr<ImTerm::terminal<terminal_commands>> terminal_log;
};

inline std::unique_ptr<console> g_console;