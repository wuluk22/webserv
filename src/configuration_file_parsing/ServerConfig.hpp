#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>
#include <set>
#include <map>

#include "ConfigException.hpp"
#include "PathValidator.hpp"

struct s_common_params {
	std::string _root;
	std::set <std::string> _index;
	bool _auto_index;
	unsigned int _client_max_body_size;
};

struct s_server_params {
	std::set<std::string> _server_name;
	std::set<unsigned int> _listen;
	std::map<unsigned int, std::string> _error_pages_record;
};

struct s_loc_params {
	std::string _cgi_path;
	std::string _uri;
	std::string _content_path;
	unsigned char _allowed_methods;
	bool _modified_client_max_body_size;
	bool _modified_auto_index;
};

enum e_allowed_methods {
	GET = 1 << 0,
	POST = 1 << 1,
	DELETE = 1 << 2
};

#ifndef IS_LINUX
	#define IS_LINUX 0
#endif

#if defined(__linux__)
    #undef IS_LINUX
    #define IS_LINUX 1
#endif

class LocationBlock;
class ServerBlock;

class ServerConfig {
	private:
		std::vector<LocationBlock *> directives;
		ServerBlock *server_header;
	public:
		ServerConfig(void);
		ServerConfig(const ServerConfig &copy);
		~ServerConfig();
		ServerConfig& operator=(const ServerConfig& rhs);
		
		std::vector<LocationBlock *> getDirectives(void) const;
		void setDirective(LocationBlock *new_directive);
		ServerBlock* getServerHeader(void) const;
		void setServerHeader(ServerBlock *server_header);
};

// Directive class sole purpose is to enable a composite design pattern for the ServerConfig class

class ADirective {
	protected:
		PathValidator _validator;
		s_common_params _common_params;
	public:
		ADirective(void);
		virtual ~ADirective();
		bool isValidFileName(std::string filename);
		s_common_params getCommonParams(void) const;

		// Individual setters
		bool setRoot(std::string root_args);
		bool setIndex(std::set <std::string> index_args);
		void setAutoIndex(bool value);
		void setClientMaxBodySize(unsigned int body_size_value);

		// Individual getters
		std::string getRoot(void) const;
		std::set <std::string> getIndex(void) const;
		bool getAutoIndex(void) const;
		unsigned int getClientMaxBodySize(void) const;
};

std::ostream& operator<<(std::ostream& os, const s_common_params *params);

class ServerBlock : public ADirective {
	private:
		s_server_params _server_params;
		bool listening_ports_set;
	public:
		ServerBlock(void);
		ServerBlock(s_common_params common_params, s_server_params server_params);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);
		s_server_params getServerParams(void) const;

		//Individual setters
		bool setServerName(std::set<std::string> server_names);
		bool setListeningPort(std::set<unsigned int> listening_ports);
		bool setErrorPagesRecord(std::map<unsigned int, std::string> error_pages_record);

		//Individual getter
		std::set<std::string> getServerName(void) const;
		std::set<unsigned int> getListeningPort(void) const;
		std::map<unsigned int, std::string> getErrorPagesRecord(void) const;
};

std::ostream& operator<<(std::ostream& os, const ServerBlock *server_params);

class LocationBlock : public ADirective {
	private:
		s_loc_params _location_params;
		LocationBlock* _upper_location;
		bool isValidExtension(std::string extension);
	public:
		LocationBlock(void);
		LocationBlock(s_common_params common_params, s_loc_params location_params);
		LocationBlock(const LocationBlock &copy);
		~LocationBlock();
		LocationBlock& operator = (const LocationBlock &rhs);
		s_loc_params getLocationParams(void) const;
		void setUpperLocation(LocationBlock* upper_location);
		void getUpperLocation(void) const;

		// Individual setter
		void clientMaxBodySizeModified(void);
		void autoIndexModified(void);
		bool setCgiPath(std::string path_args);
		bool setUri(std::string uri_args, std::string root);
		bool setAlias(std::string alias_path);
		bool setAllowedMethods(unsigned char allowed_method);
		bool setContentPath(std::string content_path);
		// Individual getter
		std::string getCgiPath(void) const;
		std::string getAlias(void) const;
		std::string getUri(void) const;
		std::string getContentPath(void) const;
		bool isCgiAllowed(void) const;
		bool isDirectiveCgi(void) const;
		bool isGetAllowed(void) const;
		bool isPostAllowed(void) const;
		bool isDeleteAllowed(void) const;
		bool hasClientMaxBodySizeModified(void) const;
		bool hasAutoIndexModified(void) const;
};

std::ostream& operator<<(std::ostream& os, const LocationBlock *location_params);

#endif