#include "settings.hpp"

#include <filesystem>

const bool settings::load(const char *config_name) {
	if (!std::filesystem::exists(std::filesystem::path(config_name)))
		throw std::invalid_argument(
		    fmt::format("File '{}' doesn't exist", config_name));

	std::ifstream inConfigFile(config_name);
	int config_line = 0;
	for (std::string line; std::getline(inConfigFile, line);) {
		config_line++;

		int equal_pos = line.find_first_of("=");
		if (equal_pos == std::string::npos)
			throw std::runtime_error(fmt::format(
			    "Settings, Invalid config, no setter (=) found, {}:{} ({})",
			    config_name, config_line, line.c_str()));

		int quote_pos = line.find_first_of("\"");
		std::string key = line.substr(0, equal_pos);
		key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

		bool isNumber = true;
		std::string value = line.substr(equal_pos + 1, line.size());
		if (quote_pos != std::string::npos) {
			int end_quote_pos = line.find_last_of("\"");

			if (!end_quote_pos)
				throw std::runtime_error(fmt::format(
				    "Settings, Invalid config, no end quote found, {}:{} ({})",
				    config_name, config_line, line.c_str()));

			if (end_quote_pos != line.size() - 1)
				throw std::runtime_error(
				    fmt::format("Settings, Invalid config, doesn't end with end quote, "
				                "{}:{} ({})",
				                config_name, config_line, line.c_str()));

			value = line.substr(quote_pos, line.size() - 1);
			isNumber = false;
		} else
			value.erase(std::remove_if(value.begin(), value.end(), ::isspace),
			            value.end());

		auto &type = vars.at(key).type();
		if (type == typeid(int)) {
			if (isNumber == false)
				throw std::runtime_error(fmt::format(
				    "Settings, Invalid config, expected int got string, {}:{} ({})",
				    config_name, config_line, line.c_str()));

			vars[key] = std::stoi(value);
		} else if (type == typeid(float)) {
			if (isNumber == false)
				throw std::runtime_error(
				    fmt::format("Settings, Invalid config, expected float got string, "
				                "{}:{} ({})",
				                config_name, config_line, line.c_str()));

			vars[key] = std::stof(value);
		} else if (type == typeid(bool)) {
			if (isNumber == false)
				throw std::runtime_error(
				    fmt::format("Settings, Invalid config, expected bool got string, "
				                "{}:{} ({})",
				                config_name, config_line, line.c_str()));

			vars[key] = (bool)std::stoi(value);
		} else if (type == typeid(const char *)) {
			if (isNumber == true)
				throw std::runtime_error(
				    fmt::format("Settings, Invalid config, expected string got number, "
				                "{}:{} ({})",
				                config_name, config_line, line.c_str()));

			vars[key] = value.c_str();
		}
	}
}

const bool settings::save(const char *config_name) {
	if (std::filesystem::exists(std::filesystem::path(config_name)))
		std::filesystem::remove(std::filesystem::path(config_name));

	std::ofstream outConfigFile(config_name, std::ios::binary);
	for (const auto &[key, value] : vars) {
		if (value.type() == typeid(int))
			outConfigFile << key << " = " << std::any_cast<int>(value) << "\n";
		else if (value.type() == typeid(bool))
			outConfigFile << key << " = " << std::any_cast<bool>(value) << "\n";
		else if (value.type() == typeid(float))
			outConfigFile << key << " = " << std::any_cast<float>(value) << "\n";
		else if (value.type() == typeid(const char *))
			outConfigFile << key << " = " << std::any_cast<const char *>(value)
			              << "\n";
		else {
			g_utils->log(
			    DEBUG, fmt::format("Settings, Unknown type {}", value.type().name()));
		}
	}
	outConfigFile.flush();
	outConfigFile.close();
}

const bool settings::init(const char *config_name) {
	if (std::filesystem::exists(std::filesystem::path(config_name)))
		load(config_name);
	else
		save(config_name);
}