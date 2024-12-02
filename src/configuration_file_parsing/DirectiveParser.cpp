#include "ConfigException.hpp"
#include "ConfigParser.hpp"

bool ConfigParser::parseRoot(std::string working_line, ADirective *directive) {
	std::string path;

	path = returnSecondArgs(working_line);
	if (path.empty())
		return (false);
	if (!directive->setRoot(path)) {
		std::cerr << ERROR_HEADER << PATH_NOT_RECOGNIZED << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool ConfigParser::parseIndex(std::vector <std::string> working_line, ADirective *directive) {
	std::set <std::string> index_file;

	working_line.erase(working_line.begin());
	if (working_line.empty()) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	for (int i = 0; i < working_line.size(); i++) {
		if (!index_file.insert(working_line[i]).second) {
			std::cerr << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
		}
	}
	return (directive->setIndex(index_file));
}

bool ConfigParser::parseAutoIndex(std::vector<std::string> args, ADirective *directive) {
	if (args.size() != 2) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	if (args[1] == "on")
		directive->setAutoIndex(true);
	else if (args[1] == "off")
		directive->setAutoIndex(false);
	else {
		std::cerr << ERROR_HEADER << INVALID_TOKEN << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool ConfigParser::parseClientMaxBodySize(std::vector <std::string> args, ADirective *directive) {
	unsigned int value;

	value = atol(args[1].c_str());
	if (value == 0 || value > UINT_MAX) {
		std::cerr << ERROR_HEADER << EXCEEDING_LIMIT << RESET << std::endl;
		return (false);
	}
	directive->setClientMaxBodySize(value);
	return (true);
}

bool ConfigParser::parseCgiPath(std::string working_line, LocationBlock *directive) {
	std::string path;

	path = returnSecondArgs(working_line);
	if (path.empty())
		return (false);
	if (!directive->setCgiPath(path)) {
		std::cerr << ERROR_HEADER << PATH_NOT_RECOGNIZED << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool ConfigParser::parseAlias(std::string working_line, LocationBlock *directive) {
	std::string path;

	path = returnSecondArgs(working_line);
	if (path.empty())
		return (false);
	if (!directive->setAlias(removeExcessiveSlashes(path))) {
		std::cerr << ERROR_HEADER << PATH_NOT_RECOGNIZED << RESET << std::endl;
		return (false);
	}
	return (true);
}

bool ConfigParser::parseAllowedMethhod(std::vector <std::string> args, LocationBlock *directive) {
	bool valid_entry;
	
	if (args.empty() || args.size() < 2 || args.size() > 4) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	args.erase(args.begin());
	for (int i = 0; i < args.size() ; i++) {
		valid_entry = false;
		if (args[i] == "GET" && !directive->isGetAllowed()) {
			valid_entry = directive->setAllowedMethods(GET);
		} else if (args[i] == "POST" && !directive->isPostAllowed()) {
			valid_entry = directive->setAllowedMethods(POST);
		} else if (args[i] == "DELETE" && !directive->isDeleteAllowed()) {
			valid_entry = directive->setAllowedMethods(DELETE);
		} if (!valid_entry) {
			std::cerr << ERROR_HEADER << BAD_INSTRUCTION << RESET << std::endl;
			return (false);
		}
	}
	return (true);
}

// Need to be checked, faulty method

bool ConfigParser::parseServerName(std::vector <std::string> args, ServerBlock *directive) {
	std::set <std::string> server_names;
	
	if (args.empty() || args.size() == 1) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	args.erase(args.begin());
	for (int i = 0; i < args.size(); i++) {
		if (!isValidServerName(args[i])) {
			std::cerr << ERROR_HEADER << "Invalid server name" << RESET << std::endl;
			return (false);
		}
		else if (!server_names.insert(args[i]).second) {
			std::cerr << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
		}
	}
	directive->setServerName(server_names);
	return (true);
}

bool ConfigParser::parseListeningPorts(std::vector <std::string> args, ServerBlock *directive) {
	std::set <unsigned int> ports;
	std::string current_string;
	unsigned int value;

	if (args.empty() || args.size() < 2) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	args.erase(args.begin());
	for (int i = 0; i < args.size(); i++) {
		if (!isStringDigit(args[i])) {
			std::cerr << ERROR_HEADER << NUMERICAL_VALUE_EXPECTED << RESET << std::endl;
			return (false);
		}
		value = std::strtoul(args[i].c_str(), NULL, 10);
		if (IS_LINUX && value < 1024) {
			std::cerr << ERROR_HEADER << RESERVED_PORTS_LINUX << RESET << std::endl;
			return (false);
		}
		if (!ports.insert(value).second) {
			std::cerr << ERROR_HEADER << DUPE_ELEMS << RESET << std::endl;
			return (false);
		}
	}
	return (directive->setListeningPort(ports));
}
bool ConfigParser::parseErrorPages(std::vector <std::string> args, ServerBlock *directive) {
	std::map <unsigned int, std::string> error_pages_record;
	std::string path;
	unsigned error_code;

	if (args.empty() || args.size() < 3) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	path = args.back();
	_validator.setPath(path);
	if (!(_validator.exists() && _validator.isFile() && _validator.isReadable())) {
		std::cerr << ERROR_HEADER << BAD_ACCESS << RESET << std::endl;
		return (false);
	}
	for (int i = 1; i < args.size() - 1; i++) {
		std::pair<unsigned int, std::string> single_error_page_record;
		error_code = std::strtoul(args[i].c_str(), NULL, 10);
		if (error_code < 400 || error_code > 527) {
			std::cerr << ERROR_HEADER << WRONG_ERROR_PAGES_SCOPE << RESET << std::endl;
			return (false);
		}
		single_error_page_record.first = error_code;
		single_error_page_record.second = path;
		error_pages_record.erase(error_code);
		error_pages_record.insert(single_error_page_record);
	}
	return (directive->setErrorPagesRecord(error_pages_record));
}

void ConfigParser::processCommonDirective(ADirective *directive, std::string working_line, std::vector<std::string> args, bool &command_status) {
	if (args[0] == "root")
		command_status = parseRoot(working_line, directive);
	else if (args[0] == "index")
		command_status = parseIndex(args, directive);
	else if (args[0] == "auto_index")
		command_status = parseAutoIndex(args, directive);
	else if (args[0] == "client_max_body_size")
		command_status = parseClientMaxBodySize(args, directive);
}


void ConfigParser::processDirectiveLoc(LocationBlock *directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	bool command_status;

	command_status = false;
	processCommonDirective(directive, working_line, args, command_status);
	if (args[0] == "cgi_path")
		command_status = parseCgiPath(working_line, directive);
	else if (args[0] == "alias")
		command_status = parseAlias(working_line, directive);
	else if (args[0] == "allowed_method")
		command_status = parseAllowedMethhod(args, directive);
	if (!command_status) {
		std::cout << ERROR_HEADER << AL << current_line << RESET << std::endl;
		throw ConfigException();
	}
}

void ConfigParser::processDirectiveServ(ServerBlock *directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	bool command_status;

	command_status = false;
	processCommonDirective(directive, working_line, args, command_status);
	if (args[0] == "server_name")
		command_status = parseServerName(args, directive);
	else if (args[0] == "listen")
		command_status = parseListeningPorts(args, directive);
	else if (args[0] == "error_pages")
		command_status = parseErrorPages(args, directive);
	if (!command_status) {
		std::cout << ERROR_HEADER << AL << current_line << RESET << std::endl;
		throw ConfigException();
	}
}
