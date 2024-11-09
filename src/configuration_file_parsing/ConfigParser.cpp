#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include <map>

ConfigParser* ConfigParser::instance = NULL;

ConfigParser::ConfigParser(const std::string init_path) {
	std::ifstream configuration_input_file;

	// LOCATION TOKENS

	l_params.push_back("cgi_path");
	l_params.push_back("cgi_extensions");
	l_params.push_back("alias");
	l_params.push_back("allowed_method");
	l_params.push_back("return");
	
	// SERVER TOKENS
	s_params.push_back("server_name");
	s_params.push_back("listen");

	// COMMON TOKENS
	c_params.push_back("root");
	c_params.push_back("index");
	c_params.push_back("auto_index");
	c_params.push_back("client_max_body_size");

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
	else
		throw std::exception();
}

bool is_token_valid_multiple(const std::string& line, const std::vector <std::string> tokens) {
	for (size_t i = 0; i < tokens.size(); ++i) {
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

std::string trim(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}

void ConfigParser::processBlock(std::ifstream &config_file, std::string w_line, TokenCounter &Tk, size_t &level) {
	std::streampos last_position;
	Tk.enterBlock();
	std::cout << "Entering block at level " << level << std::endl;

	while (std::getline(config_file, w_line)) {
		w_line = trim(w_line);
		last_position = config_file.tellg();
		
		std::cout << "current working line : " << w_line << std::endl;

		if (is_token_valid(w_line, B_LOC)) {
			std::cout << "Entering nested location block: " << w_line << std::endl;
			level++;
			processBlock(config_file, w_line, Tk, level);
			level--;
		} 
		else if (is_token_valid(w_line, LOC_TERMINATOR)) {
			std::cout << "Exiting location block at level " << level << std::endl;
			break;
		} 
		else if (is_only_whitespace(w_line)) {
			continue;
		} 
		else if (is_token_valid_multiple(w_line, l_params) || is_token_valid_multiple(w_line, c_params)) {
			Tk.incrementToken(w_line);
			std::cout << "Directive in block: " << w_line << std::endl;
			if (Tk.getTokenCount("cgi_path") > 1 || Tk.getTokenCount("cgi_extensions") > 1 || Tk.getTokenCount("root") > 1) {
				std::cerr << "Error: Directive repeated within block." << std::endl;
				std::cout << "Tk root token count: " << Tk.getTokenCount("root") << "\n";
				throw ConfigException();
			}
		} else {
			std::cout << "Invalid token encountered in block." << std::endl;
			throw ConfigException();
		}
	}
	config_file.seekg(last_position);
	Tk.exitBlock();
	std::cout << "Exited block at level " << level << std::endl;
}

void ConfigParser::parseConfigurationFile(std::ifstream &config_file) {
	std::string w_line;
	size_t level;
	ServerBlock server_directive;
	TokenCounter Tk;

	level = 0;
	while (std::getline(config_file, w_line)) {
		w_line = trim(w_line);
		if (is_token_valid(w_line, B_SERVER)) {
			level++;
			std::cout << "Starting server block at level " << level << std::endl;
		} 
		else if (is_token_valid(w_line, CONFIG_TERMINATOR) && level > 0) {
			level--;
			std::cout << "Ending server block at level " << level << std::endl;
			if (level == 0) {
				std::cout << "Exited server block" << std::endl;
			}
		} 
		else if (!level && is_only_whitespace(w_line)) {
			std::cout << "Skip empty line" << std::endl;
		} 
		else if (level > 0 && (is_token_valid_multiple(w_line, s_params) || is_token_valid_multiple(w_line, c_params))) {
			Tk.incrementToken(w_line);
			std::cout << "Server-level directive: " << w_line << std::endl;
			if (Tk.getTokenCount("root") > 1) {
				std::cerr << "Error: 'root' directive repeated within server block." << std::endl;
				throw ConfigException();
			}
		} 
		else if (level > 0 && is_token_valid(w_line, B_LOC)) {
			std::cout << "Starting location block within server block" << std::endl;
			processBlock(config_file, w_line, Tk, level);
		} 
		else {
			std::cerr << "Invalid token in configuration file: " << w_line << std::endl;
			throw ConfigException();
		}
	}
	if (level != 0) {
		std::cerr << "Error: Unmatched server or location block." << std::endl;
		throw ConfigException();
	}
	std::cout << "Finished parsing configuration file." << std::endl;
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
	} catch (ConfigException &e)
	{
		std::cout << e.what() <<std::endl;
	}
}