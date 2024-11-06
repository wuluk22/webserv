#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "ServerConfig.hpp"
#include "ConfigException.hpp"

class ConfigParser
{
	private:
		static ConfigParser* instance;
		std::string _path_of_configuration_file;
		std::map<unsigned int,std::vector<ServerConfig> > _servers_config;
		ConfigParser(const std::string init_path);
		bool endsWith(const std::string path, const std::string extension);
		void parseConfigurationFile(std::ifstream &configuration_file);
	public:
		~ConfigParser();
		ConfigParser(const ConfigParser &copy);
		static ConfigParser* getInstance(const std::string init_path);
		const std::string& getPathOfConfigurationFile(void) const;
		ServerConfig getServerConfig(unsigned int id) const;
};

#endif