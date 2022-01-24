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

#include "fmt/chrono.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/format.h"

#ifdef _WIN32
#undef ERROR  // awful MSVC fix
#endif

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

	void log(LOG_LEVELS type, std::string text) {
		if (minLogLevel <= (int)type) {
			time_t timetoday;
			time(&timetoday);
			fmt::print(fg(fmt::color::gray), "[");
			fmt::print("{:%Y-%m-%d %X} ", std::chrono::system_clock::now());
			printLogPrefix(type);
			fmt::print(fg(fmt::color::gray), "]");
			fmt::print(" {}\n", text);
		}
	};
};

inline std::unique_ptr<utils> g_utils;

#ifdef _WIN32
#define ERROR 0  // awful MSVC fix
#endif