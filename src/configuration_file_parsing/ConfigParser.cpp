#include "ConfigException.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <map>

ConfigParser* ConfigParser::instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	std::string l_items[] = { "cgi_path", "cgi_extensions", "alias", "allowed_method", "return" };
    initializeVector(l_params, l_items, ARRAY_SIZE(l_items));
    std::string s_items[] = { "server_name", "listen" };
    initializeVector(s_params, s_items, ARRAY_SIZE(s_items));
    std::string c_items[] = { "root", "index", "auto_index", "client_max_body_size" };
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
	(void) copy;
}

bool ConfigParser::endsWith(const std::string path, const std::string extension) {
	if (extension.size() > path.size())
		return (false);
	return (path.substr(path.size() - extension.size()) == extension);
}

ConfigParser::~ConfigParser() {
	delete (this->instance);
}

ConfigParser* ConfigParser::getInstance(const std::string init_path = "" ) {
	if (!instance)
		instance = new ConfigParser(init_path);
	return (instance);
}

const std::string& ConfigParser::getPathOfConfigurationFile(void) const {
	return (this->_path_of_configuration_file);
}

ServerConfig ConfigParser::getServerConfig(unsigned int id) const {
	std::map<unsigned int, std::vector<ServerConfig> >::const_iterator wanted_server_config;
	
	wanted_server_config = _servers_config.find(id);
	if (wanted_server_config != _servers_config.end() && !wanted_server_config->second.empty())
		return wanted_server_config->second.front();
	else {
		std::cerr << ERROR_HEADER << NO_SERVER_CONFIG << std::endl;
		throw ConfigException();
	}
}

void ConfigParser::processLocationBlock(std::ifstream &config_file, std::string working_line, TokenCounter &token_counter, size_t &current_line, LocationBlock *loc_directive = NULL) {
	std::vector<std::string> working_line_splitted;
	std::streampos last_position;
	LocationBlock location_directive;

	token_counter.enterBlock();
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line_splitted = split(working_line, ' ');
		if (is_token_valid(working_line_splitted[0], B_LOC)) {
			if (working_line_splitted.size() != 2) {
				std::cerr << ERROR_HEADER << NO_URI_LOCATION << AL << current_line << RESET << std::endl;  
				throw ConfigException();
			}
			processLocationBlock(config_file, working_line, token_counter, current_line);
		} else if (is_token_valid(working_line_splitted[0], LOC_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], l_params) || is_token_valid_multiple(working_line_splitted[0], c_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(non_repeat_l_token)) {
				std::cerr << ERROR_HEADER << TOKEN_REPEATED << AL << current_line << RESET << std::endl;
				throw ConfigException();
			}
			processDirectiveLoc(location_directive, working_line, working_line_splitted);
		} else {
			std::cerr << ERROR_HEADER << INVALID_TOKEN << AL << current_line << RESET << std::endl;
			throw ConfigException();
		}
	}
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::processServerBlock(std::ifstream &config_file, std::string working_line, size_t &current_line) {
	std::vector<std::string> working_line_splitted;
	std::streampos last_position;
	TokenCounter token_counter;
	ServerBlock server_directive;

	token_counter.enterBlock();
	while (std::getline(config_file, working_line)) {
		current_line++;
		last_position = config_file.tellg();
		if (is_only_whitespace(working_line))
			continue;
		working_line_splitted = split(working_line, ' ');
		if (is_token_valid(working_line_splitted[0], B_LOC)) {
			if (working_line_splitted.size() != 2) {
				std::cerr << ERROR_HEADER << NO_URI_LOCATION << AL << current_line << RESET << std::endl;  
				throw ConfigException();
			}
			processLocationBlock(config_file, working_line, token_counter, current_line);
		} else if (is_token_valid(working_line_splitted[0], SERVER_TERMINATOR) && working_line_splitted.size()) {
			break;
		} else if (is_token_valid_multiple(working_line_splitted[0], c_params) || is_token_valid_multiple(working_line_splitted[0], s_params)) {
			token_counter.incrementToken(working_line_splitted[0]);
			if (!token_counter.oneOccurenceCheck(non_repeat_s_token)) {
				std::cerr << ERROR_HEADER << TOKEN_REPEATED << AL << current_line << RESET << std::endl;
				throw ConfigException();
			}
			processDirectiveServ(server_directive, working_line, working_line_splitted);
		} else {
			std::cerr << ERROR_HEADER << INVALID_TOKEN << AL << current_line << RESET << std::endl;
			throw ConfigException();
		}
	}
	config_file.seekg(last_position);
	token_counter.exitBlock();
}

void ConfigParser::parseConfigurationFile(std::ifstream &config_file) {
	std::string working_line;
	std::vector<std::string> working_line_splitted;
	size_t current_line = 0;

	while (std::getline(config_file, working_line)) {
		current_line++;
		if (is_only_whitespace(working_line))
			continue;
		working_line_splitted = split(working_line, ' ');
		if (working_line_splitted[0] == B_SERVER && working_line.size()) {
			processServerBlock(config_file, working_line, current_line);
		} else {
			std::cerr << ERROR_HEADER << TOKEN_POSITION_MISMATCH << AL << current_line << RESET <<std::endl;
			throw ConfigException();
		}
	}
}

int main(void) {
	try {
		ConfigParser *config = ConfigParser::getInstance("test.conf");
	} catch (ConfigException &e)
	{
		std::cout << e.what() <<std::endl;
	}
}