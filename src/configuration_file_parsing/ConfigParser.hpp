#ifndef CONFIGPARSER_HPP
#define CONSIGPARSER_HPP

#include <iostream>
#include <vector>
#include <map>

#define BAD_CONFIG_FILE "The path of the configuration file you gave is not valid !"

class Directive;

class ConfigParser
{
	private:
		const std::string path_of_configuration_file;
		std::vector<Directive> all_directives;
		ConfigParser(const std::string init_path);
	public:
		ConfigParser(const ConfigParser &copy);
		ConfigParser& operator=(const ConfigParser &rhs);
		~ConfigParser();
		void createInstance(const std::string init_path);
		ConfigParser getInstance(void);
		const std::string& getPathOfConfigurationFile(void) const;
		Directive getAllDirectives(void) const;
};

#endif