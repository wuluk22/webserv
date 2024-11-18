#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>
#include <set>

#include "ConfigException.hpp"
#include "PathValidator.hpp"

struct s_common_params {
	std::string _root;
	std::vector <std::string> _index;
	bool _auto_index;
	unsigned int _client_max_body_size;
};

struct s_server_params {
	std::set<std::string> _server_name;
	std::set<unsigned int> _listen;
};

struct s_loc_params {
	bool _is_cgi;
	std::string _cgi_path;
	std::string _uri;
	std::string _alias;
	unsigned char _allowed_methods;
};

enum e_allowed_methods {
	GET = 1 << 0,
	POST = 1 << 1,
	DELETE = 1 << 2
};

#define TWO_SERVER_BLOCK_DEFINITIONS "Two server block definitions, aborting"

class ADirective;

class ServerConfig {
	private:
		std::vector<ADirective> _all_directives;
		bool _server_params_defined;
	public:
		ServerConfig(void);
		ServerConfig(const ServerConfig &copy);
		~ServerConfig();
		ServerConfig& operator=(const ServerConfig& rhs);
		std::vector<ADirective> getAllDirectives(void) const;
		void setDirective(ADirective new_directive);
		void addLocationDirective(s_common_params c_params, s_loc_params l_params);
		void addServerDirective(s_common_params c_params, s_server_params s_params);
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
		bool setRoot(std::string root_args);
		bool setIndex(std::set <std::string> index_args);
		void setAutoIndex(bool value);
		void setClientMaxBodySize(unsigned int body_size_value);
		std::string getRoot(void) const;
		s_common_params getCommonParams(void) const;
};

std::ostream& operator<<(std::ostream& os, const s_common_params& params);

class ServerBlock : public ADirective {
	private:
		s_server_params _server_params;
		bool isValidServerName(std::string name);
	public:
		ServerBlock(void);
		ServerBlock(s_common_params common_params, s_server_params server_params);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);
		s_server_params getServerParams(void) const;

		//Individual setters
		bool setServerName(std::vector<std::string> server_names);
		bool setListeningPort(std::set<unsigned int> listening_ports);
};

std::ostream& operator<<(std::ostream& os, const s_server_params& params);

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
		bool setCgiPath(std::string path_args);
		bool setUri(std::string uri_args);
		bool setAlias(std::string alias_path);
		bool setAllowedMethods(unsigned char allowed_method);

		// Allowed HTTP Methods
		bool isGetAllowed(void);
		bool isPostAllowed(void);
		bool isDeleteAllowed(void);
};

std::ostream& operator<<(std::ostream& os, const s_loc_params& params);

#endif