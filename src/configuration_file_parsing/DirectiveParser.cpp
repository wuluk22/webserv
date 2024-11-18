#include "ConfigParser.hpp"

bool ConfigParser::parseRoot(std::string working_line, ADirective directive) {
	std::string trimmed_command;
	std::string path;
	int i;

	trimmed_command = trim(working_line);
	i = trimmed_command.find(' ');
	if (i == std::string::npos) {
		std::cout << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	path = trimmed_command.substr(i + 1, std::string::npos);
	return (directive.setRoot(path));
}

bool ConfigParser::parseIndex(std::vector <std::string> working_line, ADirective directive) {
	std::set <std::string> index_file;

	working_line.erase(working_line.begin());
	if (working_line.empty()) {
		std::cout << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	for (int i = 0; i < working_line.size(); i++) {
		if (!index_file.insert(working_line[i]).second) {
			std::cout << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
		}
	}
	return (directive.setIndex(index_file));
}

bool ConfigParser::parseAutoIndex(std::vector<std::string> args, ADirective directive) {
	if (args[1] == "on")
		directive.setAutoIndex(true);
	else if (args[1] == "off")
		directive.setAutoIndex(false);
	else
		return (false);
	return (true);
}

bool ConfigParser::parseClientMaxBodySize(std::vector <std::string> args, ADirective directive) {
	unsigned int value;

	value = atol(args[1].c_str());
	if (value == 0 || value > UINT_MAX)
		return (false);
	directive.setClientMaxBodySize(value);
	return (true);
}

bool ConfigParser::parseCgiPath(std::string working_line, LocationBlock directive) {
	std::string trimmed_command;
	std::string path;
	int i;

	trimmed_command = trim(working_line);
	i = trimmed_command.find(' ');
	if (i == std::string::npos)
		return (false);
	path = trimmed_command.substr(i + 1, std::string::npos);
	return (directive.setCgiPath(path));
}

bool ConfigParser::parseAlias(std::string working_line, LocationBlock directive) {
	std::string trimmed_command;
	std::string path;
	int i;

	trimmed_command = trim(working_line);
	i = trimmed_command.find(' ');
	if (i == std::string::npos)
		return (false);
	path = trimmed_command.substr(i + 1, std::string::npos);
	return (directive.setAlias(path));
}

bool ConfigParser::parseAllowedMethhod(std::vector <std::string> args, LocationBlock directive) {
	bool valid_entry;
	
	if (args.empty() || args.size() < 2 || args.size() > 4)
		return (false);
	args.erase(args.begin());
	for (int i = 0; i < args.size() ; i++) {
		valid_entry = false;
		if (args[i] == "GET") {
			valid_entry = directive.setAllowedMethods(GET);
		} else if (args[i] == "POST") {
			valid_entry = directive.setAllowedMethods(POST);
		} else if (args[i] == "DELETE") {
			valid_entry = directive.setAllowedMethods(DELETE);
		}
		if (!valid_entry)
			return (false);
	}
	return (true);
}

bool ConfigParser::parseServerName(std::vector <std::string> args, ServerBlock directive) {
	std::set <std::string> server_names;
	
	if (args.empty())
		return (false);
	args.erase(args.begin());
	for (int i = 0; i < args.size(); i++) {
		if (!server_names.insert(args[i]).second) {
			std::cout << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
		}
	}
	return (true);
}

bool ConfigParser::parseListeningPorts(std::vector <std::string> args, ServerBlock directive) {
	std::set <unsigned int> ports;
	std::string current_string;
	unsigned int value;

	if (args.empty())
		return (false);
	args.erase(args.begin());
	for (int i = 0; i < args.size(); i++) {
		if (!isStringDigit(args[i]))
			return (false);
		value = std::strtoul(args[i].c_str(), NULL, 10);
		if (!ports.insert(value).second)
			std::cout << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
	}
	directive.setListeningPort(ports);
	return (true);
}

void ConfigParser::processDirectiveLoc(LocationBlock directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	bool command_status;

	command_status = false;
	if (args[0] == "root" && args.size() == 2)
		command_status = parseRoot(working_line, directive);
	else if (args[0] == "index" && args.size() >= 2)
		command_status = parseIndex(args, directive);
	else if (args[0] == "auto_index" && args.size() == 2)
		command_status = parseAutoIndex(args, directive);
	else if (args[0] == "client_max_body_size" && args.size() == 2)
		command_status = parseClientMaxBodySize(args, directive);
	else if (args[0] == "cgi_path" && args.size() == 2)
		command_status = parseCgiPath(working_line, directive);
	else if (args[0] == "alias" && args.size() == 2)
		command_status = parseAlias(working_line, directive);
	else if (args[0] == "allowed_method" && args.size() >= 2)
		command_status = parseAllowedMethhod(args, directive);
	if (!command_status) {
		std::cout << ERROR_HEADER << AL << current_line << RESET << std::endl;
		throw ConfigException();
	}
}

void ConfigParser::processDirectiveServ(ServerBlock directive,  std::string working_line, std::vector<std::string> args, size_t current_line) {
	bool command_status;

	command_status = false;
	if (args[0] == "root" && args.size() == 2)
		command_status = parseRoot(working_line, directive);
	else if (args[0] == "index")
		command_status = parseIndex(args, directive);
	else if (args[0] == "auto_index" && args.size() == 2)
		command_status = parseAutoIndex(args, directive);
	else if (args[0] == "client_max_body_size" && args.size() == 2)
		command_status = parseClientMaxBodySize(args, directive);
	else if (args[0] == "server_name" && args.size() >= 2)
		command_status = parseServerName(args, directive);
	else if (args[0] == "listen")
		command_status = parseListeningPorts(args, directive);
	if (!command_status) {
		std::cout << ERROR_HEADER << AL << current_line << RESET << std::endl;
		throw ConfigException();
	}
}

