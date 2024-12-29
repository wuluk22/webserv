#include "ConfigException.hpp"
#include "ConfigParser.hpp"
#include "../server_config/LocationBlock.hpp"

void ConfigParser::parseRoot(std::string working_line, ADirective *directive, size_t current_line) {
	std::string path;

	path = returnSecondArgs(working_line);
	if (path.empty())
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	if (!directive->setRoot(simplifyPath(path)))
		throw ConfigParserError(PATH_NOT_RECOGNIZED, __FUNCTION__, __LINE__, current_line);
}

void ConfigParser::parseIndex(std::vector <std::string> working_line, ADirective *directive, size_t current_line) {
	std::set <std::string> index_file;

	working_line.erase(working_line.begin());
	if (working_line.empty())
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	for (int i = 0; i < working_line.size(); i++) {
		if (!index_file.insert(working_line[i]).second)
			throw ConfigParserError(DUPE_ELEMS, __FUNCTION__, __LINE__, current_line);
	}
	directive->setIndex(index_file);
}

void ConfigParser::parseAutoIndex(std::vector<std::string> args, ADirective *directive, size_t current_line) {
	if (args.size() != 2)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	if (args[1] == "on")
		directive->setAutoIndex(true);
	else if (args[1] == "off")
		directive->setAutoIndex(false);
	else
		throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, current_line);
}

void ConfigParser::parseClientMaxBodySize(std::vector <std::string> args, ADirective *directive, size_t current_line) {
	unsigned int value;

	value = atol(args[1].c_str());
	if (value == 0 || value > UINT_MAX)
		throw ConfigParserError(EXCEEDING_LIMIT, __FUNCTION__, __LINE__, current_line);
	directive->setClientMaxBodySize(value);
}

void ConfigParser::parseCgiPath(std::string working_line, LocationBlock *directive, size_t current_line) {
	std::string path;

	path = returnSecondArgs(working_line);
	if (path.empty())
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	if (!directive->setCgiPath(path))
		throw ConfigParserError(PATH_NOT_RECOGNIZED, __FUNCTION__, __LINE__, current_line);
}

void ConfigParser::parseAlias(std::string working_line, LocationBlock *directive, size_t current_line) {
	std::string uri;

	uri = returnSecondArgs(working_line);
	if (uri.empty())
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	directive->setAlias(uri);
}

void ConfigParser::parseAllowedMethhod(std::vector <std::string> args, LocationBlock *directive, size_t current_line) {
	bool valid_entry;
	
	if (args.empty() || args.size() < 2 || args.size() > 4)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	args.erase(args.begin());
	for (int i = 0; i < args.size() ; i++) {
		valid_entry = false;
		if (args[i] == "GET" && !directive->isGetAllowed()) {
			valid_entry = directive->setAllowedMethods(GET);
		} else if (args[i] == "POST" && !directive->isPostAllowed()) {
			valid_entry = directive->setAllowedMethods(POST);
		} else if (args[i] == "DELETE" && !directive->isDeleteAllowed()) {
			valid_entry = directive->setAllowedMethods(DELETE);
		} if (!valid_entry)
			throw ConfigParserError(BAD_INSTRUCTION, __FUNCTION__, __LINE__, current_line);
	}
}

void ConfigParser::parseServerName(std::vector <std::string> args, ServerBlock *directive, size_t current_line) {
	if (args.size() != 2)
		throw ConfigParserError(SERVER_NAME_ONE_NAME, __FUNCTION__, __LINE__, current_line);
	std::string current_server = args[1];
	if (!isValidServerName(current_server))
		throw ConfigParserError(NOT_VALID_SERVER_NAME, __FUNCTION__, __LINE__, current_line);
	std::set<std::string>::iterator it = _server_names.find(current_server);
	if (it == _server_names.end())
		_server_names.insert(current_server);
	else
		throw ConfigParserError(SERVER_NAME_DUPE, __FUNCTION__, __LINE__, current_line);
	directive->setServerName(current_server);
}

void ConfigParser::parseListeningPorts(std::vector <std::string> args, ServerBlock *directive, size_t current_line) {
	std::set <unsigned int> current_working_ports;
	std::set <unsigned int>::iterator it;
	std::string current_string;
	unsigned int value;

	if (args.empty() || args.size() < 2)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	args.erase(args.begin());
	for (int i = 0; i < args.size(); i++) {
		if (!isStringDigit(args[i]))
			throw ConfigParserError(NUMERICAL_VALUE_EXPECTED, __FUNCTION__, __LINE__, current_line);
		value = std::strtoul(args[i].c_str(), NULL, 10);
		if (IS_LINUX && !(value >= 1024 && value <= 65535))
			throw ConfigParserError(PORT_SCOPE_LINUX, __FUNCTION__, __LINE__, current_line);
		else if (!(value <= 65535))
			throw ConfigParserError(PORT_SCOPE_GENERAL, __FUNCTION__, __LINE__, current_line);
		it = _ports.find(value);
		if (it == _ports.end())
			_ports.insert(value);
		else
			throw ConfigParserError(PORT_DUPE, __FUNCTION__, __LINE__, current_line);
		if (!current_working_ports.insert(value).second)
			throw ConfigParserError(DUPE_ELEMS, __FUNCTION__, __LINE__, current_line);
	}
	directive->setListeningPort(current_working_ports);
}

bool ConfigParser::checkErrorPagesAvailability(std::string path, size_t current_line) {
	_validator.setPath(path);
	if (!(_validator.exists() && _validator.isFile() && _validator.isReadable()))
		return (false);
	return (true);
}

void ConfigParser::parseErrorPages(std::vector <std::string> args, ServerBlock *directive, size_t current_line) {
	std::map <unsigned int, std::string> error_pages_record;
	std::size_t arg_size = args.size();
	std::string path;
	unsigned error_code;

	if (args.empty() || arg_size < 3)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	path = args.back();
	if (path[0] == '.')
		throw ConfigParserError(URI_STYLE_FORMAT_ERROR, __FUNCTION__, __LINE__, current_line);
	if (checkErrorPagesAvailability(directive->getRoot() + path, current_line))
		path = directive->getRoot() + path;
	else if (checkErrorPagesAvailability( directive->getRoot() + '/' + path, current_line))
		path = directive->getRoot() + '/' + path;
	else if (!checkErrorPagesAvailability(path, current_line))
		throw ConfigParserError(BAD_ACCESS, __FUNCTION__, __LINE__, current_line);
	for (int i = 1; i < arg_size - 1; ++i) {
		error_code = std::strtoul(args[i].c_str(), NULL, 10);
		if (error_code < 400 || error_code > 527) 
			throw ConfigParserError(WRONG_ERROR_PAGES_SCOPE, __FUNCTION__, __LINE__, current_line);
		error_pages_record[error_code] = path;
	}
	directive->setErrorPagesRecord(error_pages_record);
}

void ConfigParser::parseReturn(std::vector <std::string> args, LocationBlock *directive, size_t current_line) {
	std::size_t arg_size = args.size();
	std::size_t status_code;
	std::string redirection_url;

	if (args.empty() || arg_size < 2|| arg_size > 3)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	args.erase(args.begin());
	if (!isStringDigit(args[0]))
		throw ConfigParserError(NUMERICAL_VALUE_EXPECTED, __FUNCTION__, __LINE__, current_line);
	status_code = strtoul(args[0].c_str(), NULL, 10);
	if (status_code < 100 || status_code > 599)
		throw ConfigParserError(WRONG_ERROR_PAGES_SCOPE, __FUNCTION__, __LINE__, current_line);
	if (arg_size == 2)
		directive->setReturnArgs(status_code, redirection_url);
	else {
		redirection_url = args[1];
		directive->setReturnArgs(status_code, redirection_url);
	}
}

void ConfigParser::parseDependsOn(std::vector <std::string> args, LocationBlock *directive, size_t current_line) {
	std::size_t arg_size = args.size();

	if (arg_size != 2)
		throw ConfigParserError(NO_ELEMENTS, __FUNCTION__, __LINE__, current_line);
	args.erase(args.begin());
	directive->setUriDependance(args[0]);
}

void ConfigParser::processCommonDirective(ADirective *directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	if (args[0] == "root")
		parseRoot(working_line, directive, current_line);
	else if (args[0] == "index")
		parseIndex(args, directive, current_line);
	else if (args[0] == "auto_index")
		parseAutoIndex(args, directive, current_line);
	else if (args[0] == "client_max_body_size")
		parseClientMaxBodySize(args, directive, current_line);
}

void ConfigParser::processDirectiveLoc(LocationBlock *directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	processCommonDirective(directive, working_line, args, current_line);
	if (args[0] == "cgi_path")
		parseCgiPath(working_line, directive, current_line);
	else if (args[0] == "alias")
		parseAlias(working_line, directive, current_line);
	else if (args[0] == "allowed_method")
		parseAllowedMethhod(args, directive, current_line);
	else if (args[0] == "return")
		parseReturn(args, directive, current_line);
	else if (args[0] == "depends_on")
		parseDependsOn(args, directive, current_line);
}

void ConfigParser::processDirectiveServ(ServerBlock *directive, std::string working_line, std::vector<std::string> args, size_t current_line) {
	processCommonDirective(directive, working_line, args, current_line);
	if (args[0] == "server_name")
		parseServerName(args, directive, current_line);
	else if (args[0] == "listen")
		parseListeningPorts(args, directive, current_line);
	else if (args[0] == "error_pages")
		parseErrorPages(args, directive, current_line);
}