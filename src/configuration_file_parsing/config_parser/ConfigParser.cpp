#include "ConfigParser.hpp"
#include "ConfigException.hpp"
#include "../server_config/ServerConfig.hpp"

ConfigParser* ConfigParser::_instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	// Weird INIT hack, should check a more clean way to initialize those token elements
	std::string l_items[] = { "cgi_path", "alias", "allowed_method", "return"};
    initializeVector(_l_params, l_items, ARRAY_SIZE(l_items));
    std::string s_items[] = { "server_name", "listen", "error_pages" };
    initializeVector(_s_params, s_items, ARRAY_SIZE(s_items));
    std::string c_items[] = { "root", "index", "auto_index", "client_max_body_size", "access_log", "error_log"};
    initializeVector(_c_params, c_items, ARRAY_SIZE(c_items));
    std::string non_repeat_s[] = { "root" };
    initializeVector(_non_repeat_s_token, non_repeat_s,ARRAY_SIZE(non_repeat_s));
    std::string non_repeat_l[] = { "root", "cgi_path" };
    initializeVector(_non_repeat_l_token, non_repeat_l, ARRAY_SIZE(non_repeat_l));

	if (init_path.empty() || !endsWith(init_path, FILE_EXT))
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	this->_path_of_configuration_file = init_path;
	configuration_input_file.open(init_path.c_str());
	if (!configuration_input_file.is_open())
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	parseConfigurationFile(configuration_input_file);
}

ConfigParser::~ConfigParser() {
	for (int i = 0; i < _servers_config.size(); i++)
		delete _servers_config[i];
	delete this->_instance;
}

ConfigParser* ConfigParser::getInstance(const std::string init_path = "") {
	if (!_instance)
		_instance = new ConfigParser(init_path);
	return (_instance);
}

const std::string& ConfigParser::getPathOfConfigurationFile(void) const {
	return (this->_path_of_configuration_file);
}

ServerConfig* ConfigParser::getServerConfig(unsigned int id) const {
	std::map <size_t, ServerConfig *>::const_iterator it;

	it = this->_servers_config.find(id);
	if (it != this->_servers_config.end()) {
		return (it->second);
	}
	throw ConfigParserError(NO_SERVER_CONFIG, __FUNCTION__, __LINE__);
}

std::map <size_t, ServerConfig *> ConfigParser::getAllServerConfig(void) const {
	return (this->_servers_config);
}

void ConfigParser::setServerConfig(size_t server_id, ServerConfig *current_server) {
	this->_servers_config.insert(std::make_pair(server_id, current_server));
}

// TODO : REFACTOR THIS MONSTROSITY P1

void ConfigParser::finalizeLocationBlock(LocationBlock *directive, ServerBlock *server_config, std::string uri, size_t line) {
	std::string server_root = server_config->getRoot();
	std::string location_root = directive->getRoot();
	std::string root;
	std::set<std::string> default_index;

	if (location_root.empty() && server_root.empty())
		throw ConfigParserError(NO_ROOT_DEFINITION, __FUNCTION__, __LINE__, line);
	else if (!server_root.empty() && location_root.empty())
		directive->setRoot(server_root);
	root = directive->getRoot();
	if (!directive->setContentPath(removeExcessiveSlashes(root + uri)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
	if (!directive->setUri(removeExcessiveSlashes(uri), removeExcessiveSlashes(root)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
 	if (directive->getIndex().empty() && !server_config->getIndex().empty())
		directive->setIndex(server_config->getIndex());
	if (!directive->hasAutoIndexModified())
		directive->setAutoIndex(server_config->getAutoIndex());
	if (!directive->hasClientMaxBodySizeModified())
		directive->setClientMaxBodySize(server_config->getClientMaxBodySize());
	_validator.setPath(directive->getUri());
	if (!_validator.isDirectory())
		directive->setAutoIndex(0);
}

// TODO : REFACTOR THIS MONSTROSITY P1

void ConfigParser::processLocationBlock(std::ifstream &config_file, std::string working_line, TokenCounter &token_counter, size_t &current_line, ServerBlock *current_server, ServerConfig *server_config) {
	std::vector<std::string> working_line_splitted;
	std::streampos last_position;
	LocationBlock *location_directive = new LocationBlock();
	s_parser_flags flag = (s_parser_flags) {false, false, false, false, false};
	std::string uri;
	size_t uri_line;

	token_counter.enterBlock();
	uri_line = current_line;
	server_config->setDirective(location_directive);
	uri = returnSecondArgs(working_line);
	if (!distinctUri(uri, server_config))
		throw ConfigParserError(DOUBLE_LOCATION_URI, __FUNCTION__, __LINE__, current_line);	
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line = trim(working_line);
		working_line_splitted = split(working_line, ' ');
		flag.alias_defined = (working_line_splitted[0] == "alias") ? true : flag.alias_defined;
		flag.root_defined = (working_line_splitted[0] == "root") ? true : flag.root_defined;
		if (flag.root_defined && flag.alias_defined)
			throw ConfigParserError(AMBIGUOUS_URI_DEFINITION, __FUNCTION__, __LINE__, current_line);
		if (is_token_valid(working_line_splitted[0], B_LOC)) {
			if (working_line_splitted.size() != 2)
				throw ConfigParserError(NO_URI_LOCATION, __FUNCTION__, __LINE__, current_line);
			if (location_directive->getRoot().empty()) {
				if (current_server->getRoot().empty())
					throw ConfigParserError(NO_ROOT_DEFINITION, __FUNCTION__, __LINE__, current_line);
				location_directive->setRoot(current_server->getRoot());
			}
			processLocationBlock(config_file, working_line, token_counter, current_line, current_server, server_config);
			flag.went_in_directive = true;
		} else if (is_token_valid(working_line_splitted[0], LOC_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], _l_params) || is_token_valid_multiple(working_line_splitted[0], _c_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(_non_repeat_l_token))
				throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, current_line);
			if (flag.went_in_directive && working_line_splitted[0] == "root")
				throw ConfigParserError(ROOT_PRIORITY, __FUNCTION__, __LINE__, current_line);
			processDirectiveLoc(location_directive, working_line, working_line_splitted, current_line);
			(working_line_splitted[0] == "client_max_body_size") ? location_directive->clientMaxBodySizeModified() :
			(working_line_splitted[0] == "auto_index") ? location_directive->autoIndexModified() : void();

		} else
			throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, current_line);
	}
	finalizeLocationBlock(location_directive, current_server, uri, uri_line);
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::processServerBlock(std::ifstream &config_file, std::string working_line, size_t &current_line, ServerConfig *server_config) {
	std::vector<std::string> working_line_splitted;
	std::streampos last_position;
	TokenCounter token_counter;
	ServerBlock *server_directive = new ServerBlock();

	token_counter.enterBlock();
	server_config->setServerHeader(server_directive);
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line = trim(working_line);
		working_line_splitted = split(working_line, ' ');
		if (is_token_valid(working_line_splitted[0], B_LOC)) {
			if (working_line_splitted.size() != 2)
				throw ConfigParserError(NO_URI_LOCATION, __FUNCTION__, __LINE__, current_line);
			processLocationBlock(config_file, working_line, token_counter, current_line, server_directive, server_config);
		} else if (is_token_valid(working_line_splitted[0], SERVER_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], _c_params) || is_token_valid_multiple(working_line_splitted[0], _s_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(_non_repeat_s_token))
				throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, current_line);
			processDirectiveServ(server_directive, working_line, working_line_splitted, current_line);
		} else
			throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, current_line);
	}
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::parseConfigurationFile(std::ifstream &config_file) {
	std::string working_line;
	std::vector<std::string> working_line_splitted;
	size_t current_line = 0;
	size_t server_id = 0;

	while (std::getline(config_file, working_line)) {
		ServerConfig *server_config = new ServerConfig();
		current_line++;
		if (is_only_whitespace(working_line))
			continue;
		working_line_splitted = split(working_line, ' ');
		if (working_line_splitted[0] == B_SERVER && working_line.size()) {
			processServerBlock(config_file, working_line, current_line, server_config);
		} else {
			delete server_config;
			throw ConfigParserError(TOKEN_POSITION_MISMATCH, __FUNCTION__, __LINE__, current_line);
		}
		setServerConfig(server_id, server_config);
		server_id++;
	}
}

bool ConfigParser::distinctUri(std::string current_uri, ServerConfig *current_server) {
	std::vector <LocationBlock *> all_directives = current_server->getDirectives();
	if (current_uri.empty())
		return (false);
	for (std::vector<LocationBlock *>::iterator it = all_directives.begin(); it != all_directives.end(); it++) {
		if ((*it)->getUri() == current_uri)
			return (false);
	}
	return (true);
}
