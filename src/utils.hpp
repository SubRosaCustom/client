#pragma once

#include <stdarg.h>
#include <stdio.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/chrono.h"
#include "fmt/format.h"

enum LOG_LEVELS {
	DEBUG,
	INFO,
	WARN,
	ERROR,
};

class utils {
 public:
	int minLogLevel;

	utils(LOG_LEVELS level) : minLogLevel(level){};

	std::string_view getLogLevelColor(LOG_LEVELS level) {
		fg(fmt::color::crimson);
	}

	void printLogPrefix(LOG_LEVELS level) {
		switch (level) {
			case DEBUG:
				fmt::print(fg(fmt::color::blue), "Debug");
				break;
			case INFO:
				fmt::print(fg(fmt::color::green), "Info");
				break;
			case WARN:
				fmt::print(fg(fmt::color::yellow), "Warn");
				break;
			case ERROR:
				fmt::print(fg(fmt::color::red), "Error");
				break;
			default:
				fmt::print(fg(fmt::color::red), "Unk");
				break;
		}
	}

	template <typename... Args>
	void log(LOG_LEVELS type, std::string_view text, Args... args) {
		if (minLogLevel <= (int)type) {
			time_t timetoday;
   			time (&timetoday);
			fmt::print(fg(fmt::color::gray), "[");
			fmt::print("{:%Y-%m-%d %X} ", std::chrono::system_clock::now());
			printLogPrefix(type);
			fmt::print(fg(fmt::color::gray), "]");
			fmt::print(" {}\n", text, args...);
			// fmt::print(fg(fmt::color::crimson), "{}", LEVEL)
			// fmt::print(fg(fmt::color::), "] {}", text, args...);
			// fmt::vformat("{}", args);

			// auto now = std::chrono::system_clock::now();
			// auto in_time_t = std::chrono::system_clock::to_time_t(now);
			// std::cout << "[\x1B[90m"
			        //   << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d|%X ")
			        //   << getLogPrefix(type) << "] \033[0m" << text << '\n';
		}
	};
};

inline std::unique_ptr<utils> g_utils;