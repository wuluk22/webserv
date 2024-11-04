#include "ConfigParser.hpp"

// TODO : Need to implement a parser to give the attributes values
// Need to implement a custom Exception class for error handling
ConfigParser::ConfigParser(std::string config_file_path) {
	bool status;
	// status = configFileParsing(config_file_path);
	// if (!status)
	//    throw UnvalidParsedFile(BAD_CONFIG_FILE);
}

ConfigParser::~ConfigParser(void) {

}

ConfigParser::ConfigParser(const ConfigParser &copy) {
	(*this) = copy;
}

ConfigParser& ConfigParser::operator=(const ConfigParser &assign) {
	if (this != &assign) {
		this->root_dir = assign.getRootDirectory();
		this->log_file = assign.getLogFile();
		this->virtual_hosts = assign.getVirtualHosts();
		this->mime_types = assign.getMimeTypes();
		this->error_pages_directory = assign.getErrorPagesDirectory();
		this->enable_directory_listening = assign.isDirectoryListeningEnabled();
		this->enable_cgi = assign.isCgiEnabled();
	}
	return (*this);
}

std::string ConfigParser::getRootDirectory(void) const {
	return (this->root_dir);
}

std::string ConfigParser::getLogFile(void) const {
	return (this->log_file);
}

std::map<std::string, std::string> ConfigParser::getVirtualHosts(void) const {
	return (this->virtual_hosts);
}

std::map<std::string, std::string> ConfigParser::getMimeTypes(void) const {
	return (this->mime_types);
}

std::string ConfigParser::getErrorPagesDirectory(void) const {
	return (this->error_pages_directory);
}

bool ConfigParser::isDirectoryListeningEnabled(void) const {
	return (this->enable_directory_listening);
}

bool ConfigParser::isCgiEnabled(void) const {
	return (this->enable_cgi);
}
