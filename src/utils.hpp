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

	std::string_view getLogPrefix(LOG_LEVELS level) {
		switch (level) {
			case DEBUG:
				return "\x1B[32mDebug";
				break;
			case INFO:
				return "\x1B[34mInfo";
				break;
			case WARN:
				return "\x1B[33mWarn";
				break;
			case ERROR:
				return "\x1B[31mError";
				break;

			default:
				return "\x1b[31mUnk";
				break;
		}
	}

	void log(LOG_LEVELS type, const char* text, ...) {
		if (minLogLevel <= (int)type) {
			char buffer[256];
			va_list list;
			va_start(list, text);
			vsnprintf(buffer, 255, text, list);
			auto now = std::chrono::system_clock::now();
			auto in_time_t = std::chrono::system_clock::to_time_t(now);
			std::cout << "[\x1B[90m"
			          << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d|%X ")
			          << getLogPrefix(type) << "] \033[0m" << buffer << '\n';
			va_end(list);
		}
	};
};

inline std::unique_ptr<utils> g_utils;

#ifdef _WIN32
#define ERROR 0 // awful MSVC fix
#endif