#include "ConfigParser.hpp"
#include "ConfigException.hpp"
#include "../server_config/ServerConfig.hpp"

ConfigParser* ConfigParser::_instance = NULL;

/**
 * @brief Construct a new Config Parser:: Config Parser object
 * Initialize vectors of allowed tokens as well as unrepeatable
 * tokens in the context of either locations block or server blocks
 * @param initPath The config file path
 */
ConfigParser::ConfigParser(const std::string initPath) {
	std::ifstream configurationInputFile;

	initializeVector(_c_params, C_TOKENS);
	initializeVector(_l_params, L_BLOCK_TOKENS);
	initializeVector(_s_params, S_BLOCK_TOKENS);
	initializeVector(_non_repeat_tokens_l, L_NO_RPT_TOKENS);
	initializeVector(_non_repeat_tokens_s, S_NO_RPT_TOKENS);
	if (initPath.empty() || !endsWith(initPath, FILE_EXT))
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	this->_path_of_configuration_file = initPath;
	configurationInputFile.open(initPath.c_str());
	if (!configurationInputFile.is_open())
		throw ConfigParserError(BAD_CONFIG_FILE, __FUNCTION__, __LINE__);
	parseConfigurationFile(configurationInputFile);
}

ConfigParser::~ConfigParser() {
	for (int i = 0; i < _servers_config.size(); i++)
		delete _servers_config[i];
	delete this->_instance;
}

ConfigParser* ConfigParser::getInstance(const std::string initPath = "") {
	if (!_instance)
		_instance = new ConfigParser(initPath);
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

void ConfigParser::finalizeLocationBlock(LocationBlock *directive, ServerBlock *serverConfig, std::string uri, size_t line) {
	std::string server_root = serverConfig->getRoot();
	std::string location_root = directive->getRoot();
	std::string root;
	std::set<std::string> defaultIndex; // inutilise???

	if (location_root.empty() && server_root.empty())
		throw ConfigParserError(NO_ROOT_DEFINITION, __FUNCTION__, __LINE__, line);
	else if (!server_root.empty() && location_root.empty())
		directive->setRoot(server_root);
	root = directive->getRoot();
	if (!directive->setContentPath(removeExcessiveSlashes(root + uri)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
	if (!directive->setUri(removeExcessiveSlashes(uri), removeExcessiveSlashes(root)))
		throw ConfigParserError(BAD_URI, __FUNCTION__, __LINE__, line);
 	if (directive->getIndex().empty() && !serverConfig->getIndex().empty())
		directive->setIndex(serverConfig->getIndex());
	if (!directive->hasAutoIndexModified())
		directive->setAutoIndex(serverConfig->getAutoIndex());
	if (!directive->hasClientMaxBodySizeModified())
		directive->setClientMaxBodySize(serverConfig->getClientMaxBodySize());
	_validator.setPath(directive->getUri());
	if (!_validator.isDirectory())
		directive->setAutoIndex(0);
}

bool ConfigParser::handleLocationTokens(std::vector<std::string> tokens, std::string workingLine, LocationBlock *l_directive, 
		s_parser_flags &flag, size_t line_num, ServerBlock *curr_serv, ServerConfig *serv_conf, std::ifstream &configFile, TokenCounter &tokenCounter) {
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
		processLocationBlock(configFile, workingLine, tokenCounter, line_num, curr_serv, serv_conf);
		flag.went_in_directive = true;
		return (true);
	}
	return (false);
}

bool ConfigParser::handleDirectiveTokens(std::vector<std::string> tokens, std::string workingLine, LocationBlock *location_directive,
		TokenCounter &tokenCounter, s_parser_flags &flag, size_t currentLine) {
	if (tokens.empty())
		return (false);
	if (is_token_valid_multiple(tokens[0], _l_params) || is_token_valid_multiple(tokens[0], _c_params)) {
		tokenCounter.incrementToken(tokens[0]);
		if (!tokenCounter.oneOccurenceCheck(_non_repeat_tokens_l))
			throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, currentLine);
		else if (flag.went_in_directive && tokens[0] == "root")
			throw ConfigParserError(ROOT_PRIORITY, __FUNCTION__, __LINE__, currentLine);
		processDirectiveLoc(location_directive, workingLine, tokens, currentLine);
		if (tokens[0] == "client_max_body_size")
			location_directive->clientMaxBodySizeModified();
		else if (tokens[0] == "auto_index")
			location_directive->autoIndexModified();
		return (true);
	}
	return (false);
}

void ConfigParser::processLocationBlock(std::ifstream &configFile, std::string workingLine, TokenCounter &tokenCounter, 
		size_t &currentLine, ServerBlock *current_server, ServerConfig *serverConfig) {
	LocationBlock *l_directive = new LocationBlock();
	std::vector<std::string> splittedLine;
	s_parser_flags flag = {false, false, false, false, false};
	std::string uri;
	size_t uriLine;
	std::streampos lastPosition;

	tokenCounter.enterBlock();
	serverConfig->setDirective(l_directive);
	uriLine = currentLine;
	uri = returnSecondArgs(workingLine);
	if (!distinctUri(uri, serverConfig))
		throw ConfigParserError(DOUBLE_LOCATION_URI, __FUNCTION__, __LINE__, currentLine);
	while (std::getline(configFile, workingLine)) {
		currentLine++;
		lastPosition = configFile.tellg();
		if (is_only_whitespace(workingLine))
			continue;
		workingLine = trim(workingLine);
		splittedLine = split(workingLine, ' ');
		if (handleLocationTokens(splittedLine, workingLine, l_directive, flag, currentLine, current_server, serverConfig, configFile, tokenCounter))
			continue;
		else if (is_token_valid(splittedLine[0], LOC_TERMINATOR))
			break;
		else if (handleDirectiveTokens(splittedLine, workingLine, l_directive, tokenCounter, flag, currentLine))
			continue;
		throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, currentLine);
	}
	finalizeLocationBlock(l_directive, current_server, uri, uriLine);
	configFile.seekg(lastPosition);
	tokenCounter.exitBlock();
	std::cout << l_directive << std::endl;
}

void ConfigParser::processServerBlock(std::ifstream &configFile, std::string workingLine, size_t &currentLine, ServerConfig *serverConfig) {
	std::vector<std::string> splittedLine;
	std::streampos lastPosition;
	TokenCounter tokenCounter;
	ServerBlock *s_directive = new ServerBlock();

	tokenCounter.enterBlock();
	serverConfig->setServerHeader(s_directive);
	while (std::getline(configFile, workingLine)) {
		currentLine++;
		lastPosition = configFile.tellg();
		if (is_only_whitespace(workingLine))
			continue;
		workingLine = trim(workingLine);
		splittedLine = split(workingLine, ' ');
		if (is_token_valid(splittedLine[0], B_LOC)) {
			if (splittedLine.size() != 2)
				throw ConfigParserError(NO_URI_LOCATION, __FUNCTION__, __LINE__, currentLine);
			processLocationBlock(configFile, workingLine, tokenCounter, currentLine, s_directive, serverConfig);
		} else if (is_token_valid(splittedLine[0], SERVER_TERMINATOR) && splittedLine.size()) {
			break;
		} else if (is_token_valid_multiple(splittedLine[0], _c_params) || is_token_valid_multiple(splittedLine[0], _s_params)) {
			tokenCounter.incrementToken(splittedLine[0]);
			if (!tokenCounter.oneOccurenceCheck(_non_repeat_tokens_s))
				throw ConfigParserError(TOKEN_REPEATED, __FUNCTION__, __LINE__, currentLine);
			processDirectiveServ(s_directive, workingLine, splittedLine, currentLine);
		} else
			throw ConfigParserError(INVALID_TOKEN, __FUNCTION__, __LINE__, currentLine);
	}
	configFile.seekg(lastPosition);
	tokenCounter.exitBlock();
	std::cout << s_directive << std::endl;
}

void ConfigParser::parseConfigurationFile(std::ifstream &configFile) {
	std::string workingLine;
	std::vector<std::string> workingLineSplitted;
	size_t currentLine = 0;
	size_t server_id = 0;

	while (std::getline(configFile, workingLine)) {
		ServerConfig *serverConfig = new ServerConfig();
		currentLine++;
		if (is_only_whitespace(workingLine))
			continue;
		workingLineSplitted = split(workingLine, ' ');
		if (workingLineSplitted[0] == B_SERVER && workingLine.size()) {
			processServerBlock(configFile, workingLine, currentLine, serverConfig);
		} else {
			delete serverConfig;
			throw ConfigParserError(TOKEN_POSITION_MISMATCH, __FUNCTION__, __LINE__, currentLine);
		}
		setServerConfig(server_id, serverConfig);
		server_id++;
	}
}

bool ConfigParser::distinctUri(std::string current_uri, ServerConfig *current_server) {
	std::vector <LocationBlock *> allDirectives = current_server->getDirectives();
	if (current_uri.empty())
		return (false);
	for (std::vector<LocationBlock *>::iterator it = allDirectives.begin(); it != allDirectives.end(); it++) {
		if ((*it)->getUri() == current_uri)
			return (false);
	}
	return (true);
}
