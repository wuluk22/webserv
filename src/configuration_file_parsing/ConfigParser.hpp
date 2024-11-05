#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "Directive.hpp"

#define BAD_CONFIG_FILE "The path of the configuration file you gave is not valid !"

class ConfigParser
{
	private:
		std::string _path_of_configuration_file;
		std::map<unsigned int,std::vector<Directive>> _servers_config;
		ConfigParser(const std::string init_path);
	public:
		ConfigParser(const ConfigParser &copy) = delete;
		ConfigParser& operator=(const ConfigParser &rhs) = delete;
		~ConfigParser();
		ConfigParser& createInstance(const std::string init_path);
		ConfigParser& getInstance(void);
		const std::string& getPathOfConfigurationFile(void) const;
		Directive getServerConfig(unsigned int id) const;
		class ConfigException : public std::exception {
			public:
				virtual const char* what() const throw();
		};
};

#endif