#ifndef CONFIGPARSER_HPP
#define CONSIGPARSER_HPP

#include <iostream>
#include <map>

#define BAD_CONFIG_FILE "The configuration file you gave is not valid"

class ConfigParser
{
	private:
		std::string root_dir;
		std::string log_file;
		std::map<std::string, std::string> virtual_hosts;
		std::map<std::string, std::string> mime_types;
		bool enable_directory_listening;
		std::string error_pages_directory;
		bool enable_cgi;
	public:
		ConfigParser(std::string config_file_path);
		~ConfigParser(void);
		ConfigParser(const ConfigParser &copy);
		ConfigParser& operator=(const ConfigParser &assign);

		// Getter
		std::string getRootDirectory(void) const;
		std::string getLogFile(void) const;
		std::map<std::string, std::string> getVirtualHosts(void) const;
		std::map<std::string, std::string> getMimeTypes(void) const;
		std::string getErrorPagesDirectory(void) const;
		bool isDirectoryListeningEnabled(void) const;
		bool isCgiEnabled(void) const;
};

#endif