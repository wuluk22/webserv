#include "ConfigParser.hpp"
#include "Directive.hpp"
#include <map>

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	if (!init_path.empty())
		throw ConfigParser::ConfigException();
	configuration_input_file.open(init_path);
	if (!configuration_input_file.is_open())
		throw ConfigParser::ConfigException();
	// TODO : Envoyer le fichier au casse pipe
}

ConfigParser::~ConfigParser() {}

ConfigParser& ConfigParser::createInstance(const std::string init_path) {

}

ConfigParser& ConfigParser::getInstance(void) {

}

const std::string& ConfigParser::getPathOfConfigurationFile(void) const {
	return (this->_path_of_configuration_file);
}

Directive ConfigParser::getServerConfig(unsigned int id) const {
	std::map<unsigned int, std::vector<Directive> >::const_iterator wanted_server_config;
	
	wanted_server_config = _servers_config.find(id);
	if (wanted_server_config != _servers_config.end() && !wanted_server_config->second.empty())
		return wanted_server_config->second.front();
	else
		throw ConfigParser::ConfigException();
}

const char* ConfigParser::ConfigException::what() const throw() {
	// TODO : message d'erreur
}