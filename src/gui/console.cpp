#include "console.hpp"
#include "../game.hpp"

constexpr std::array<terminal_helper::command_type, 2> local_command_list{
    terminal_helper::command_type{"clear", "clears the terminal screen",
                                  terminal_helper::clear,
                                  terminal_helper::no_completion},
    terminal_helper::command_type{"memedit", "memory edit a section of memory",
                                  terminal_helper::memedit, terminal_helper::no_completion},
};

// clears the logging screen. argument_type is aliased in
// ImTerm::basic_terminal_helper
void terminal_helper::clear(argument_type& arg) { arg.term.clear(); }

void terminal_helper::help(argument_type& arg) {
	constexpr unsigned long list_element_name_max_size =
	    misc::max_size(local_command_list.begin(), local_command_list.end(),
	                   [](const command_type& cmd) { return cmd.name.size(); });

	arg.term.add_text("Available commands:");
	for (const command_type& cmd : local_command_list) {
		arg.term.add_text(spdlog::fmt_lib::format("        {:{}} | {}", cmd.name,
		                  list_element_name_max_size, cmd.description));
	}
	arg.term.add_text("");
	arg.term.add_text(
	    "Additional information might be available using \"'command' --help\"");
}

void terminal_helper::memedit(argument_type& arg) {
	ImTerm::message msg;
	if (arg.command_line.size() < 2) {
		arg.term.add_text_err(
		    "memedit takes 2 parameters, memedit <address> <amount_to_show>");
		return;
	}
	try {
		g_memoryEditor->memData =
		    (void*)(g_game->getBaseAddress() +
		            std::stoi(arg.command_line[1], nullptr, 0));
		g_memoryEditor->memSize = std::stoi(arg.command_line[2], nullptr, 0);

		if (!g_memoryEditor->isOpen) g_memoryEditor->isOpen = true;
	} catch (const std::exception& e) {
		std::string error =
		    spdlog::fmt_lib::format("Invalid parameter, {}", e.what());
		msg.value = std::move(error);
		msg.color_beg = 1;
		arg.term.add_message(msg);
	}
}

terminal_helper::terminal_helper() {
	for (const command_type& cmd : local_command_list) {
		add_command_(cmd);
	}
}