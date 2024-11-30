#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stack>
#include <limits.h>
#include <stdlib.h>
#include <cctype>
#include "ServerConfig.hpp"

// MACROS
#define SERVER_TERMINATOR "end-server"
#define LOC_TERMINATOR "end-location"
#define B_SERVER "server"
#define B_LOC "location"
#define FILE_EXT ".conf"

// ARRAY SIZE MACRO
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// IS LINUX FLAG - FOR RESERVED PORTS

#ifndef IS_LINUX
	#define IS_LINUX 0
#endif

#if defined(__linux__)
    #undef IS_LINUX
    #define IS_LINUX 1
#endif

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

typedef struct s_parser_flags {
	bool went_in_directive;
	bool root_defined;
	bool alias_defined;
	bool cgi_allowed_defined;
	bool cgi_path_defined;
}t_parser_flags;

class ConfigParser {
	private:
		std::vector <std::string> l_params;
		std::vector <std::string> c_params;
		std::vector <std::string> s_params;
		std::vector <std::string> non_repeat_l_token;
		std::vector <std::string> non_repeat_s_token;
		static ConfigParser* instance;
		std::string _path_of_configuration_file;
		std::map<size_t , ServerConfig *> _servers_config;
		PathValidator _validator;
		
		// Constructor
		ConfigParser(const std::string init_path);

		// Methods utils
		std::string trim(const std::string& str);
		std::vector<std::string> split(const std::string& str, char delimiter);
		bool is_only_whitespace(const std::string& str);
		bool is_token_valid_multiple(const std::string& line, const std::vector <std::string> tokens);
		bool is_token_valid(const std::string& line, const std::string token);
		void initializeVector(std::vector<std::string>& vec, std::string items[], size_t count);
		bool endsWith(const std::string path, const std::string extension);
		bool isStringDigit(std::string args);
		bool isValidServerName(std::string name);
		std::string returnSecondArgs(std::string args);
		bool checkPathLocationDirective(LocationBlock *location_block);
		std::string removeExcessiveSlashes(const std::string& path);

		// Main methods
		void parseConfigurationFile(std::ifstream &configuration_file);
		void processLocationBlock(std::ifstream &config_file, std::string w_line, TokenCounter &Tk,  size_t &current_line, ServerBlock *current_server, ServerConfig *server_config);
		void processServerBlock(std::ifstream &config_file, std::string w_line,  size_t &current_line, ServerConfig *server_config);
		void processCommonDirective(ADirective *directive, std::string working_line, std::vector<std::string> args, bool &command_status);
		void processDirectiveLoc(LocationBlock *directive, std::string working_line, std::vector<std::string> args, size_t current_line);
		void processDirectiveServ(ServerBlock *directive,  std::string working_line, std::vector<std::string> args, size_t current_line);
		bool finalizeLocationBlock(LocationBlock *directive, ServerBlock *server_config, std::string location_line);
		
		// Parsing methods for each tokens
		// Common
		bool parseRoot(std::string working_line, ADirective *directive);
		bool parseIndex(std::vector <std::string> working_line, ADirective *directive);
		bool parseAutoIndex(std::vector<std::string> args, ADirective *directive);
		bool parseClientMaxBodySize(std::vector <std::string> args, ADirective *directive);
	
		// Location
		bool parseCgiPath(std::string working_line, LocationBlock *directive);
		bool parseAlias(std::string working_line, LocationBlock *directive);
		bool parseAllowedMethhod(std::vector <std::string> args, LocationBlock *directive);
		bool parseReturn(std::vector <std::string> args,LocationBlock *directive);

		// Server
		bool parseServerName(std::vector <std::string> args, ServerBlock *directive);
		bool parseListeningPorts(std::vector <std::string> args, ServerBlock *directive);
	public:
		~ConfigParser();
		ConfigParser(const ConfigParser &copy);
		static ConfigParser* getInstance(const std::string init_path);
		const std::string& getPathOfConfigurationFile(void) const;
		ServerConfig* getServerConfig(unsigned int id) const;
		std::map <size_t, ServerConfig *> getAllServerConfig(void) const;
		void setServerConfig(size_t server_id, ServerConfig *current_server);
		bool areAllPathAccessible(ServerConfig *current_server_config);
};

#endif