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

enum LOG_LEVELS {
	DEBUG,
	INFO,
	WARN,
	ERROR,
};

class utils {
 public:
	void log(LOG_LEVELS type, const char* text, ...) {
		char buffer[256];
		va_list list;
		va_start(list, text);
		vsnprintf(buffer, 255, text, list);
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		std::cout << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d|%X  ")
		          << "LOL"
		          << "]  " << buffer << std::endl;
		va_end(list);
	};
};

inline std::unique_ptr<utils> g_utils;