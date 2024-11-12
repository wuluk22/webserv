#include "ConfigParser.hpp"

std::vector<std::string> ConfigParser::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	
	for (std::size_t i = 0; i < str.size(); ++i) {
		if (str[i] == delimiter) {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
		} else {
			token += str[i];
		}
	}
	if (!token.empty()) {
		tokens.push_back(token);
	}
	return tokens;
}

std::string ConfigParser::trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}

bool ConfigParser::is_only_whitespace(const std::string& str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (!std::isspace(str[i])) {
			return false;
		}
	}
	return true;
}

bool ConfigParser::is_token_valid_multiple(const std::string& line, const std::vector <std::string> tokens) {
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (line.find(tokens[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool ConfigParser::is_token_valid(const std::string& line, const std::string token) {
	return line == token;
}

bool ConfigParser::isTwo(std::vector <std::string> arguments) {
	return (arguments.size())
}


