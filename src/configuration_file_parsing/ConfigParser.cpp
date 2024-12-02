#include "ConfigException.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"

ConfigParser* ConfigParser::instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	std::string l_items[] = { "cgi_path", "alias", "allowed_method"};
    initializeVector(l_params, l_items, ARRAY_SIZE(l_items));
    std::string s_items[] = { "server_name", "listen", "error_pages" };
    initializeVector(s_params, s_items, ARRAY_SIZE(s_items));
    std::string c_items[] = { "root", "index", "auto_index", "client_max_body_size"};
    initializeVector(c_params, c_items, ARRAY_SIZE(c_items));
    std::string non_repeat_s[] = { "root" };
    initializeVector(non_repeat_s_token, non_repeat_s,ARRAY_SIZE(non_repeat_s));
    std::string non_repeat_l[] = { "root", "cgi_extensions", "cgi_path" };
    initializeVector(non_repeat_l_token, non_repeat_l, ARRAY_SIZE(non_repeat_l));

	if (init_path.empty() || !endsWith(init_path, FILE_EXT))
		throw ConfigException();
	this->_path_of_configuration_file = init_path;
	configuration_input_file.open(init_path.c_str());
	if (!configuration_input_file.is_open())
		throw ConfigException();
	parseConfigurationFile(configuration_input_file);
}

ConfigParser::ConfigParser(const ConfigParser &copy) {
	*this = copy;
}

ConfigParser::~ConfigParser() {
	for (int i = 0; i < _servers_config.size(); i++) {
		delete _servers_config[i];
	}
	delete this->instance;
}

ConfigParser* ConfigParser::getInstance(const std::string init_path = "") {
	if (!instance)
		instance = new ConfigParser(init_path);
	return (instance);
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
	std::cout << ERROR_HEADER << NO_SERVER_CONFIG << RESET << std::endl;
	throw ConfigException();
}

std::map <size_t, ServerConfig *> ConfigParser::getAllServerConfig(void) const {
	return (this->_servers_config);
}

void ConfigParser::setServerConfig(size_t server_id, ServerConfig *current_server) {
	this->_servers_config.insert(std::make_pair(server_id, current_server));
}

bool ConfigParser::finalizeLocationBlock(LocationBlock *directive, ServerBlock *server_config, std::string uri) {
	std::string server_root = server_config->getRoot();
	std::string location_root = directive->getRoot();
	std::string root;
	std::set<std::string> default_index;

	if (location_root.empty() && server_root.empty()) {
		std::cerr << ERROR_HEADER << NO_ROOT_DEFINITION << RESET << std::endl;
		return (false);
	} else if (!server_root.empty() && location_root.empty())
		directive->setRoot(server_root);
	root = directive->getRoot();
	if (directive->getContentPath().empty())
		directive->setContentPath(removeExcessiveSlashes(root + uri));
	if (!directive->setUri(removeExcessiveSlashes(uri), removeExcessiveSlashes(root))) {
		std::cerr << ERROR_HEADER << BAD_URI << RESET << std::endl;
		return (false);
	}
	if (directive->getIndex().empty() && server_config->getIndex().empty()) {
		default_index.insert("index.html");
		default_index.insert("index.htm");
		directive->setIndex(default_index);
	} else if (directive->getIndex().empty() && !server_config->getIndex().empty())
		directive->setIndex(server_config->getIndex());
	if (!directive->hasAutoIndexModified())
		directive->setAutoIndex(server_config->getAutoIndex());
	if (!directive->hasClientMaxBodySizeModified())
		directive->setClientMaxBodySize(server_config->getClientMaxBodySize());
	return (true);
}

void ConfigParser::processLocationBlock(std::ifstream &config_file, std::string working_line, TokenCounter &token_counter, size_t &current_line, ServerBlock *current_server, ServerConfig *server_config) {
	std::vector<std::string> working_line_splitted;
	std::streampos last_position;
	LocationBlock *location_directive = new LocationBlock();
	s_parser_flags flag;
	std::string uri;

	flag.alias_defined = false;
	flag.cgi_allowed_defined = false;
	flag.cgi_path_defined = false;
	flag.root_defined = false;
	flag.went_in_directive = false;
	uri = returnSecondArgs(working_line);
	if (!distinctUri(uri, server_config)) {
		std::cerr << ERROR_HEADER << DOUBLE_LOCATION_URI << AL << current_line << RESET << std::endl;
		throw ConfigException();
	}
	token_counter.enterBlock();
	server_config->setDirective(location_directive);
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line = trim(working_line);
		working_line_splitted = split(working_line, ' ');
		if (working_line_splitted[0] == "alias")
			flag.alias_defined = true;
		else if (working_line_splitted[0] == "root")
			flag.root_defined = true;
		if (flag.root_defined && flag.alias_defined) {
			std::cerr << ERROR_HEADER << AMBIGUOUS_URI_DEFINITION << AL << current_line << RESET << std::endl;
			throw ConfigException();
		}
		if (is_token_valid(working_line_splitted[0], B_LOC)) {
			if (working_line_splitted.size() != 2) {
				std::cerr << ERROR_HEADER << NO_URI_LOCATION << AL << current_line << RESET << std::endl;  
				throw ConfigException();
			}
			if (location_directive->getRoot().empty()) {
				if (current_server->getRoot().empty()) {
					std::cerr << ERROR_HEADER << NO_ROOT_DEFINITION << AL << current_line << RESET << std::endl;
					throw ConfigException();
				}
				location_directive->setRoot(current_server->getRoot());
			}
			processLocationBlock(config_file, working_line, token_counter, current_line, current_server, server_config);
			flag.went_in_directive = true;
		} else if (is_token_valid(working_line_splitted[0], LOC_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], l_params) || is_token_valid_multiple(working_line_splitted[0], c_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(non_repeat_l_token)) {
				std::cerr << ERROR_HEADER << TOKEN_REPEATED << AL << current_line << RESET << std::endl;
				throw ConfigException();
			}
			if (flag.went_in_directive && working_line_splitted[0] == "root") {
				std::cerr << ERROR_HEADER << ROOT_PRIORITY << AL << current_line << RESET << std::endl;
				throw ConfigException();
			}
			processDirectiveLoc(location_directive, working_line, working_line_splitted, current_line);
			if (working_line_splitted[0] == "client_max_body_size")
				location_directive->clientMaxBodySizeModified();
			else if (working_line_splitted[0] == "auto_index")
				location_directive->autoIndexModified();
		} else {
			std::cerr << ERROR_HEADER << INVALID_TOKEN << AL << current_line << RESET << std::endl;
			throw ConfigException();
		}
	}
	if (!finalizeLocationBlock(location_directive, current_server, uri))
		throw ConfigException();
	config_file.seekg(last_position);
	token_counter.exitBlock();
	std::cout << location_directive << std::endl;
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
			if (working_line_splitted.size() != 2) {
				std::cerr << ERROR_HEADER << NO_URI_LOCATION << AL << current_line << RESET << std::endl;  
				throw ConfigException();
			}
			processLocationBlock(config_file, working_line, token_counter, current_line, server_directive, server_config);
		} else if (is_token_valid(working_line_splitted[0], SERVER_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], c_params) || is_token_valid_multiple(working_line_splitted[0], s_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(non_repeat_s_token)) {
				std::cerr << ERROR_HEADER << TOKEN_REPEATED << AL << current_line << RESET << std::endl;
				throw ConfigException();
			}
			processDirectiveServ(server_directive, working_line, working_line_splitted, current_line);
		} else {
			std::cerr << ERROR_HEADER << INVALID_TOKEN << AL << current_line << RESET << std::endl;
			throw ConfigException();
		}
	}
	config_file.seekg(last_position);
	token_counter.exitBlock();
	std::cout << server_directive << std::endl;
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
			std::cerr << ERROR_HEADER << TOKEN_POSITION_MISMATCH << AL << current_line << RESET <<std::endl;
			throw ConfigException();
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

bool ConfigParser::checkPathLocationDirective(LocationBlock *location_block) {
	return (true);
}

bool ConfigParser::areAllPathAccessible(ServerConfig *current_server_config) {
	std::vector<LocationBlock *> all_directives;
	if (!current_server_config)
		return (false);
	all_directives = current_server_config->getDirectives();
	for (int i = 0; i < all_directives.size(); i++) {
		if (!checkPathLocationDirective(all_directives[i]))
			return (false);
	}
	return (true);
}

// int main(void) {
// 	ConfigParser *config;
// 	try {
// 		 config = ConfigParser::getInstance("test.conf");
// 	} catch (ConfigException &e)
// 	{
// 		std::cout << e.what() <<std::endl;
// 	}
// 	// ServerConfig* c = config->getServerConfig(0);
// }