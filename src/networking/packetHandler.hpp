#pragma once

#include <any>
#include <exception>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"

class packetHandler {
 private:
	std::string buffer;
	int seek;

 public:
	packetHandler(std::string _buffer = "", int _seek = 0)
	    : buffer(_buffer), seek(_seek){};
	~packetHandler(){};

	std::string readString(size_t size) {
		if (seek + size > buffer.length())
			throw std::out_of_range("No more data to seek.");

		std::string bytes;
		bytes.assign(buffer, seek, size);
		seek += size;

		return bytes;
	}

	template <typename T>
	T read() {
		if (seek + (sizeof(T) * 2) > buffer.length())
			throw std::out_of_range("No more data to seek.");

		std::string bytes;
		bytes.assign(buffer, seek, sizeof(T) * 2);
		seek += sizeof(T) * 2;

		T num;
		sscanf(bytes.c_str(), "%x", &num);

		return num;
	}

	template <typename T>
	void write(T value) {
		std::ostringstream ss;
		ss << std::setw(sizeof(T) * 2) << std::setfill('0') << std::hex
		   << *((int32_t*)&value);
		buffer = buffer + ss.str();
	}

	std::string get() { return buffer; }
	
	std::string getRest() { std::string temp; temp.assign(buffer, seek); return std::move(temp); }
};