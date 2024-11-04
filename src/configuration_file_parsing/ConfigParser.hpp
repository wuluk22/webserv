#ifndef CONFIGPARSER_HPP
#define CONSIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "Directive.hpp"

#define BAD_CONFIG_FILE "The path of the configuration file you gave is not valid !"

class ConfigParser
{
	private:
		const std::string path_of_configuration_file;
		std::map<unsigned int,std::vector<Directive>> servers_config;
		ConfigParser(const std::string init_path);
	public:
		ConfigParser(const ConfigParser &copy) = delete;
		ConfigParser& operator=(const ConfigParser &rhs) = delete;
		~ConfigParser();
		ConfigParser& createInstance(const std::string init_path);
		ConfigParser& getInstance(void);
		std::string& getPathOfConfigurationFile(void) const;
		std::map<unsigned int, std::vector<Directive>> getServersConfig(void) const;
};

#endif