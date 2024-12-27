#include "ConfigParser.hpp"
#include "ConfigException.hpp"
#include "../server_config/ServerConfig.hpp"

ConfigParser* ConfigParser::_instance = NULL;

/**
 * @brief Construct a new Config Parser:: Config Parser object
 * Initialize vectors of allowed tokens as well as unrepeatable
 * tokens in the context of either locations block or server blocks
 * @param init_path The config file path
 */
ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	initializeVector(_c_params, C_TOKENS);
	initializeVector(_l_params, L_BLOCK_TOKENS);
	initializeVector(_s_params, S_BLOCK_TOKENS);
	initializeVector(_non_repeat_tokens_l, L_NO_RPT_TOKENS);
	initializeVector(_non_repeat_tokens_s, S_NO_RPT_TOKENS);
	if (init_path.empty() || !endsWith(init_path, FILE_EXT))
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	configuration_input_file.open(init_path.c_str());
	if (!configuration_input_file.is_open())
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	parseConfigurationFile(configuration_input_file);
}

ConfigParser::~ConfigParser() {
	for (int i = 0; i < _servers_config.size(); i++) {
		_logger.info("Removing server " + toStrInt(i) + ".");
		delete _servers_config[i];
	}
	delete this->_instance;
}

ConfigParser* ConfigParser::getInstance(const std::string init_path = "") {
	if (!_instance)
		_instance = new ConfigParser(init_path);
	return (_instance);
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

void ConfigParser::finalizeServerBlock(ServerBlock *directive, size_t line, ServerConfig *serv_conf , size_t server_id) {
	_logger.info("Finalizing server " + toStrInt(server_id + 1) + " parsing");
	if (!directive->wasListeningPortSet())
		throw ConfigParserError(PORT_NOT_SET, __FUNCTION__, __LINE__, line);
	if (directive->getServerName().empty())
		throw ConfigParserError(SERVER_NAME_NOT_SET, __FUNCTION__, __LINE__, line);
	if (directive->getRoot().empty())
		_logger.warn("Relying on root definition for each location directive - not recommanded");
	if (serv_conf->getDirectives().empty())
		_logger.warn("No directive set up for the current server");
}

void ConfigParser::finalizeLocationBlock(LocationBlock *directive, ServerBlock *server_config, std::string uri, size_t line) {
	std::string server_root = server_config->getRoot();
	std::string location_root = directive->getRoot();
	std::string root;
	std::set<std::string> default_index;

	if (location_root.empty() && server_root.empty())
		throw ConfigParserError(NO_ROOT_DEFINITION, __FUNCTION__, __LINE__, line);
	if (!server_root.empty() && location_root.empty())
		directive->setRoot(server_root);
	root = directive->getRoot();
	if (!directive->setContentPath(removeExcessiveSlashes(root + uri)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
	if (!directive->setUri(removeExcessiveSlashes(uri), removeExcessiveSlashes(root)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
	if (uri.find("..") != std::string::npos)
        throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
 	if (directive->getIndex().empty() && !server_config->getIndex().empty())
		directive->setIndex(server_config->getIndex());
	if (!directive->hasAutoIndexModified())
		directive->setAutoIndex(server_config->getAutoIndex());
	if (!directive->hasClientMaxBodySizeModified())
		directive->setClientMaxBodySize(server_config->getClientMaxBodySize());
	_validator.setPath(directive->getContentPath());
	if (!_validator.isDirectory())
		directive->setAutoIndex(0);
}

bool ConfigParser::handleLocationTokens(std::vector<std::string> tokens, std::string working_line, LocationBlock *l_directive, 
		s_parser_flags &flag, size_t line_num, ServerBlock *curr_serv, ServerConfig *serv_conf, std::ifstream &config_file, TokenCounter &token_counter) {
	if (tokens.empty())
		return (false);
	if (is_token_valid(tokens[0], B_LOC)) {
		if (tokens.size() != 2)
			throw ConfigParserError(NO_URI_LOCATION, __FUNCTION__, __LINE__, line_num);
		if (l_directive->getRoot().empty()) {
			if (curr_serv->getRoot().empty())
				throw ConfigParserError(NO_ROOT_DEFINITION, __FUNCTION__, __LINE__, line_num);
			l_directive->setRoot(curr_serv->getRoot());
		}
		processLocationBlock(config_file, working_line, token_counter, line_num, curr_serv, serv_conf);
		flag.went_in_directive = true;
		return (true);
	}
	return (false);
}

bool ConfigParser::handleDirectiveTokens(std::vector<std::string> tokens, std::string working_line, LocationBlock *location_directive,
		TokenCounter &token_counter, s_parser_flags &flag, size_t current_line) {
	if (tokens.empty())
		return (false);
	if (is_token_valid_multiple(tokens[0], _l_params) || is_token_valid_multiple(tokens[0], _c_params)) {
		token_counter.incrementToken(tokens[0]);
		if (!token_counter.oneOccurenceCheck(_non_repeat_tokens_l))
			throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, current_line);
		else if (flag.went_in_directive && tokens[0] == "root")
			throw ConfigParserError(ROOT_PRIORITY, __FUNCTION__, __LINE__, current_line);
		processDirectiveLoc(location_directive, working_line, tokens, current_line);
		if (tokens[0] == "client_max_body_size")
			location_directive->clientMaxBodySizeModified();
		else if (tokens[0] == "auto_index")
			location_directive->autoIndexModified();
		return (true);
	}
	return (false);
}

void ConfigParser::processLocationBlock(std::ifstream &config_file, std::string working_line, TokenCounter &token_counter, 
		size_t &current_line, ServerBlock *current_server, ServerConfig *server_config) {
	LocationBlock *l_directive = new LocationBlock();
	std::vector<std::string> splitted_line;
	s_parser_flags flag = {false, false, false, false, false};
	std::string uri;
	size_t uri_line;
	std::streampos last_position;

	token_counter.enterBlock();
	server_config->setDirective(l_directive);
	uri_line = current_line;
	uri = returnSecondArgs(working_line);
	if (!distinctUri(uri, server_config))
		throw ConfigParserError(DOUBLE_LOCATION_URI, __FUNCTION__, __LINE__, current_line);
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line = trim(working_line);
		splitted_line = split(working_line, ' ');
		if (handleLocationTokens(splitted_line, working_line, l_directive, flag, current_line, current_server, server_config, config_file, token_counter))
			continue;
		else if (is_token_valid(splitted_line[0], LOC_TERMINATOR))
			break;
		else if (handleDirectiveTokens(splitted_line, working_line, l_directive, token_counter, flag, current_line))
			continue;
		throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, current_line);
	}
	finalizeLocationBlock(l_directive, current_server, uri, uri_line);
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::processServerBlock(std::ifstream &config_file, std::string working_line, size_t &current_line, ServerConfig *server_config, size_t server_id) {
	std::vector<std::string> splitted_line;
	std::streampos last_position;
	TokenCounter token_counter;
	ServerBlock *s_directive = new ServerBlock();

	token_counter.enterBlock();
	server_config->setServerHeader(s_directive);
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line = trim(working_line);
		splitted_line = split(working_line, ' ');
		if (is_token_valid(splitted_line[0], B_LOC)) {
			if (splitted_line.size() != 2)
				throw ConfigParserError(NO_URI_LOCATION, __FUNCTION__, __LINE__, current_line);
			processLocationBlock(config_file, working_line, token_counter, current_line, s_directive, server_config);
		} else if (is_token_valid(splitted_line[0], SERVER_TERMINATOR) && splitted_line.size()) {
			break;
		} else if (is_token_valid_multiple(splitted_line[0], _c_params) || is_token_valid_multiple(splitted_line[0], _s_params)) {
			token_counter.incrementToken(splitted_line[0]);
			if (!token_counter.oneOccurenceCheck(_non_repeat_tokens_s))
				throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, current_line);
			processDirectiveServ(s_directive, working_line, splitted_line, current_line);
		} else
			throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, current_line);
	}
	finalizeServerBlock(s_directive, current_line, server_config, server_id);
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::parseConfigurationFile(std::ifstream &config_file) {
	std::string working_line;
	std::vector<std::string> working_line_splitted;
	size_t current_line = 0;
	size_t server_id = 0;

	while (std::getline(config_file, working_line)) {
		ServerConfig *server_config;
		Logger current_logger;
		current_line++;
		if (is_only_whitespace(working_line))
			continue;
		working_line_splitted = split(working_line, ' ');
		if (working_line_splitted[0] == B_SERVER && working_line.size()) {
			server_config = new ServerConfig();
			processServerBlock(config_file, working_line, current_line, server_config, server_id);
		} else {
			delete server_config;
			throw ConfigParserError(TOKEN_POSITION_MISMATCH, __FUNCTION__, __LINE__, current_line);
		}
		setServerConfig(server_id, server_config);
		server_id++;
	}
}

bool ConfigParser::distinctUri(std::string current_uri, ServerConfig *current_server) {
    if (!current_uri.empty() && current_uri[current_uri.size() - 1] == '/' && current_uri.size() != 1)
        current_uri = current_uri.substr(0, current_uri.size() - 1);
    std::vector<LocationBlock *> all_directives = current_server->getDirectives();
    if (current_uri.empty())
        return false;
    for (std::vector<LocationBlock *>::iterator it = all_directives.begin(); it != all_directives.end(); ++it) {
        std::string existing_uri = (*it)->getUri();
        if (!existing_uri.empty() && existing_uri[existing_uri.size() - 1] == '/')
            existing_uri = existing_uri.substr(0, existing_uri.size() - 1);
        if (current_uri == "/" && existing_uri == "/")
            return false;
        if (existing_uri == current_uri)
            return false; 
	}
    return true;
}
