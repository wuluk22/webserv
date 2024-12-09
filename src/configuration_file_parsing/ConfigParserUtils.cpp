#include "ConfigParser.hpp"

bool ConfigParser::endsWith(const std::string path, const std::string extension) {
	if (extension.size() > path.size())
		return (false);
	return (path.substr(path.size() - extension.size()) == extension);
}

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
	return (tokens);
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
			return (true);
		}
	}
	return false;
}

bool ConfigParser::is_token_valid(const std::string& line, const std::string token) {
	return line == token;
}

void ConfigParser::initializeVector(std::vector<std::string>& vec, std::string items[], size_t count) {
    for (size_t i = 0; i < count; ++i) {
        vec.push_back(items[i]);
    }
}

bool ConfigParser::isStringDigit(std::string args) {
	if (args.empty())
		return (false);
	for (int i = 0; i < args.size(); i++) {
		if (!std::isdigit(args[i]))
			return (false);
	}
	return (true);
}

bool ConfigParser::isValidServerName(std::string name) {
	bool last_dot_occurence;

	last_dot_occurence = false;
	if (name.empty())
		return (false);
	if (!std::isalpha(name[0]))
		return (false);
	for (int i = 0; i < name.size(); i++) {
		char c = name[i];
		if (std::isalnum(c) || c == '-' || c == '.') {
			if ((i == 0 || i == name.size() - 1) && (c == '.' || c == '-')) 
				return (false);
			if (c == '.') {
				if (last_dot_occurence) 
					return (false);
				last_dot_occurence = true;
			} else
				last_dot_occurence = false;
		} else
			return (false);
	}
	return (true);
}

std::string ConfigParser::returnSecondArgs(std::string args) {
	std::string trimmed_line;
	std::string uri;
	std::size_t i;

	trimmed_line = trim(args);
	i = trimmed_line.find(' ');
	if (i == std::string::npos) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return ("");
	}
	uri = trimmed_line.substr(i + 1, std::string::npos);
	return (uri);
}