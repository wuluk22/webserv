#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <map>

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	if (!init_path.empty() || !endsWith(init_path, ".conf"))
		throw ConfigException(BAD_CONFIG_FILE);
	configuration_input_file.open(init_path);
	if (!configuration_input_file.is_open())
		throw ConfigException(NO_ACCESS);
	
}

bool ConfigParser::endsWith(const std::string path, const std::string extension) {
	if (extension.size() > path.size())
		return (false);
	return (path.substr(path.size() - extension.size()) == extension);
}

ConfigParser::~ConfigParser() {
	delete (this->instance);
}

ConfigParser& ConfigParser::getInstance(const std::string init_path = "") {
	if (!instance)
		instance = new ConfigParser(init_path);
	return (*instance);
}

const std::string& ConfigParser::getPathOfConfigurationFile(void) const {
	return (this->_path_of_configuration_file);
}

Directive ConfigParser::getServerConfig(unsigned int id) const {
	std::map<unsigned int, std::vector<ServerConfig> >::const_iterator wanted_server_config;
	
	wanted_server_config = _servers_config.find(id);
	if (wanted_server_config != _servers_config.end() && !wanted_server_config->second.empty())
		return wanted_server_config->second.front();
	else
		throw ConfigException(NO_SERVER_CONFIG);
}

// EXCEPTION CLASS

ConfigParser::ConfigException::ConfigException(const std::string& msg) : message(msg) {}

const char* ConfigParser::ConfigException::what() const throw() {
	return (message.c_str());
}