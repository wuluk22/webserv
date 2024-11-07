#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <map>

ConfigParser* ConfigParser::instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	if (init_path.empty() || !endsWith(init_path, ".conf"))
		throw BadPathException();
	this->_path_of_configuration_file = init_path;
	configuration_input_file.open(init_path.c_str());
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

bool is_token_valid_multiple(const std::string& line, const std::string tokens[], size_t size) {
	for (size_t i = 0; i < size; ++i) {
		if (line.find(tokens[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool is_token_valid(const std::string& line, const std::string token) {
	return line == token;
}

bool is_only_whitespace(const std::string& str) {
	for (size_t i = 0; i < str.size(); ++i) {
		if (!std::isspace(str[i])) {
			return false;
		}
	}
	return true;
}

#define C_SIZE 4
#define S_SIZE 2
#define L_SIZE 4
#define TAB_SIZE 4

/**
 *  PARAMS THAT CAN ONLY BE DELCARED ONE TIME IN A RESPECTIVE BLOCK 
 *  ROOT - CGI-PATH - CGI-EXTENSIONS - allowed_method
 *  
 */
std::string trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}

void ConfigParser::parseConfigurationFile(std::ifstream &configuration_file) {
	std::string working_line;
	std::string possible_directive;
	std::string c_param[4] = {"root", "index", "auto_index", "client_max_body_size"};
	std::string s_param[2] = {"server_name", "listen"};
	std::string l_param[4] = {"cgi_path", "cgi_extensions", "alias", "allowed_methods"};
	unsigned int level = 0;
	ADirective workingDirective;
	TokenCounter Tk;

	while (std::getline(configuration_file, working_line)) {
		working_line = trim(working_line);
		if (level == 0 && is_token_valid(working_line,B_SERVER)) {
			level++;
			std::cout << "inside" << std::endl;
		} else if (level && is_token_valid(working_line, CONFIG_TERMINATOR)) {
			std::cout << "outside" << std::endl;
			level--;
		} else if (!level && is_only_whitespace(working_line)){
			std::cout << "skip" << "\n";
		} else if (level && level && (is_token_valid_multiple(working_line, s_param, S_SIZE) || is_token_valid_multiple(working_line, c_param, C_SIZE))) {
			Tk.incrementToken(working_line);
			std::cout << "good directive" << std::endl;
			if (Tk.getTokenCount("root") > 1) {
				std::cout << "a bit too much" << std::endl;
				break;
			}
		} else if (level && is_token_valid(working_line, B_LOC)) {
			std::cout << "entering location" << std::endl;
		} else {
			std::cout << "bye bye" << std::endl;
			break;
		}	
	}
}

// Token counter class

TokenCounter::TokenCounter() {
	blockStack.push(std::map<std::string, int>());
}

void TokenCounter::enterBlock() {
	blockStack.push(std::map<std::string, int>());
}

void TokenCounter::exitBlock() {
	if (blockStack.size() > 1) {
		blockStack.pop();
	} else {
		std::cerr << "Error: No block to exit!" << std::endl;
	}
}

void TokenCounter::incrementToken(const std::string& token) {
	blockStack.top()[token]++;
}

int TokenCounter::getTokenCount(const std::string& token) {
	return blockStack.top()[token];
}

int main(void) {
	try {
		ConfigParser *config = ConfigParser::getInstance("test.conf");
	} catch (BadPathException &e)
	{
		std::cout << e.what() <<std::endl;
	}
}