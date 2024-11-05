#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "Directive.hpp"

#define BAD_CONFIG_FILE "Invalid file"
#define NO_ACCESS "Cannot acces the file"
#define NO_SERVER_CONFIG "ID points to non-existant server configuration"

class ConfigParser
{
	private:
		ConfigParser* instance;
		std::string _path_of_configuration_file;
		std::map<unsigned int,std::vector<Directive>> _servers_config;
		ConfigParser(const std::string init_path);
		bool endsWith(const std::string path, const std::string extension);
	public:
		ConfigParser(const ConfigParser &copy) = delete;
		ConfigParser& operator=(const ConfigParser &rhs) = delete;
		~ConfigParser();
		ConfigParser& getInstance(const std::string init_path);
		const std::string& getPathOfConfigurationFile(void) const;
		Directive getServerConfig(unsigned int id) const;
		class ConfigException : public std::exception {
			private:
				std::string message;
			public:
				ConfigException(const std::string& msg);
				virtual const char* what() const throw();
		};
};

#endif