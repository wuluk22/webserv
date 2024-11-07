#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stack>
#include "ServerConfig.hpp"
#include "ConfigException.hpp"

#define CONFIG_TERMINATOR "end-server"
#define B_SERVER "server"
#define B_LOC "location"

class ConfigParser
{
	private:
		static ConfigParser* instance;
		std::string _path_of_configuration_file;
		std::map<unsigned int,std::vector<ServerConfig> > _servers_config;
		ConfigParser(const std::string init_path);
		bool endsWith(const std::string path, const std::string extension);
		void parseConfigurationFile(std::ifstream &configuration_file);
		void processBlock(std::ifstream &configuration_file, std::string working_line);
	public:
		~ConfigParser();
		ConfigParser(const ConfigParser &copy);
		static ConfigParser* getInstance(const std::string init_path);
		const std::string& getPathOfConfigurationFile(void) const;
		ServerConfig getServerConfig(unsigned int id) const;
};

class TokenCounter {
	public:
		TokenCounter();
		void enterBlock();
		void exitBlock();
		void incrementToken(const std::string& token);
		int getTokenCount(const std::string& token);
	private:
		std::stack<std::map<std::string, int> > blockStack;
};

#endif