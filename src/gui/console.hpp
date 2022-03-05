#pragma once

#include <array>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "../utils/utils.hpp"
#include "gui.hpp"
#include "imterm/misc.hpp"
#include "imterm/terminal.hpp"
#include "imterm/terminal_helpers.hpp"
#include "imterm/utils.hpp"
#include "memoryEditor.hpp"

struct console_options {
	bool should_close = false;
};

class terminal_helper : public ImTerm::basic_spdlog_terminal_helper<
                            terminal_helper, console_options, misc::no_mutex> {
 public:
	terminal_helper();

	static std::vector<std::string> no_completion(argument_type&) { return {}; }
	static void clear(argument_type& arg);
	static void help(argument_type& arg);
	static void memedit(argument_type& arg);
};

inline std::unique_ptr<console_options> g_console_options;
inline std::unique_ptr<ImTerm::terminal<terminal_helper>> g_console;