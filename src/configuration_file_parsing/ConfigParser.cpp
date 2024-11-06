#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <map>

ConfigParser* ConfigParser::instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	if (init_path.empty() || !endsWith(init_path, ".conf"))
		throw BadPathException();
	this->_path_of_configuration_file = init_path;
	configuration_input_file.open(init_path);
	if (!configuration_input_file.is_open())
		throw NoAccessException();
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

ConfigParser* ConfigParser::getInstance(const std::string init_path = "") {
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
	else
		throw std::exception();
}

void ConfigParser::parseConfigurationFile(std::ifstream &configuration_file) {
	(void) configuration_file;
}

int main(void) {
	try {
		ConfigParser *config = ConfigParser::getInstance("test.con");
	} catch (BadPathException &e)
	{
		std::cout << e.what() <<std::endl;
	}
	
}