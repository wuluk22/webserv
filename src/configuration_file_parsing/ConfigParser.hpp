#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stack>
#include "ServerConfig.hpp"
#include "ConfigException.hpp"

// MACROS
#define SERVER_TERMINATOR "end-server"
#define LOC_TERMINATOR "end-location"
#define B_SERVER "server"
#define B_LOC "location"
#define FILE_EXT ".conf"

// ARRAY SIZE MACRO
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

class TokenCounter {
	private:
		std::stack<std::map<std::string, int> > blockStack;
	public:
		TokenCounter();
		void enterBlock();
		void exitBlock();
		void incrementToken(const std::string& token);
		int getTokenCount(const std::string& token) const;
		bool oneOccurenceCheck(std::vector <std::string> unrepeatable_tokens);
};

class ConfigParser {
	private:
		std::vector <std::string> l_params;
		std::vector <std::string> c_params;
		std::vector <std::string> s_params;
		std::vector <std::string> non_repeat_l_token;
		std::vector <std::string> non_repeat_s_token;
		static ConfigParser* instance;
		std::string _path_of_configuration_file;
		std::map<unsigned int,std::vector<ServerConfig> > _servers_config;
		
		// Constructor
		ConfigParser(const std::string init_path);

		// Methods utils
		std::string trim(const std::string& str);
		std::vector<std::string> split(const std::string& str, char delimiter);
		bool is_only_whitespace(const std::string& str);
		bool is_token_valid_multiple(const std::string& line, const std::vector <std::string> tokens);
		bool is_token_valid(const std::string& line, const std::string token);
		bool isTwo(std::vector <std::string> arguments);

		// Main methods
		void initializeVector(std::vector<std::string>& vec, std::string items[], size_t count);
		bool endsWith(const std::string path, const std::string extension);
		void parseConfigurationFile(std::ifstream &configuration_file);
		void processLocationBlock(std::ifstream &config_file, std::string w_line, TokenCounter &Tk,  size_t &current_line, LocationBlock *loc_directive);
		void processServerBlock(std::ifstream &config_file, std::string w_line,  size_t &current_line);
		void processDirectiveLoc(LocationBlock directive, std::vector <std::string> args);
		void processDirectiveServ(ServerBlock directive, std::vector <std::string> args);
	public:
		~ConfigParser();
		ConfigParser(const ConfigParser &copy);
		static ConfigParser* getInstance(const std::string init_path);
		const std::string& getPathOfConfigurationFile(void) const;
		ServerConfig getServerConfig(unsigned int id) const;
};

#endif