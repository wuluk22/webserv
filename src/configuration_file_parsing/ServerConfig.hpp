#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>
#include "ConfigException.hpp"

class ADirective;

struct s_common_params {
	std::string _root;
	std::string _index;
	bool _auto_index;
	unsigned int _client_max_body_size;
};

struct s_server_params {
	std::vector<std::string> _server_name;
	std::vector<unsigned int> _listen;
};

struct s_cgi_params {
	std::string _cgi_path;
	std::vector <std::string> _cgi_extension;
};

struct s_loc_params {
	bool _is_cgi;
	s_cgi_params _cgi_params;
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
	public:
		ADirective(void);
};

class ServerBlock : public ADirective {
	private:
		s_common_params _common_params;
		s_server_params _server_params;
	public:
		ServerBlock(void);
		ServerBlock(s_common_params common_params, s_server_params server_params);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);
		s_common_params getCommonParams(void) const;
		s_server_params getServerParams(void) const;
		void setCommonParams(s_common_params c_params);
		void setServerParams(s_server_params s_params);

		//Individual setters
		void setRoot(std::string root_args);
		void setIndex(std::string index_args);
		void setAutoIndex(bool value);
		void setClientMaxBodySize(unsigned int body_size_value);
};

class LocationBlock : public ADirective {
	private:
		s_common_params _common_params;
		s_loc_params _location_params;
		LocationBlock* _upper_location;
	public:
		LocationBlock(void);
		LocationBlock(s_common_params common_params, s_loc_params location_params);
		LocationBlock(const LocationBlock &copy);
		~LocationBlock();
		LocationBlock& operator = (const LocationBlock &rhs);
		s_common_params getCommonParams(void) const;
		s_loc_params getLocationParams(void) const;
		void setUpperLocation(LocationBlock* upper_location);
		void setCommonParams(s_common_params common_params);
		void setLocationParams(s_loc_params location_params);

		// Individual setter
		void setIsCgi(bool value);
		void setCgiPath(std::string path_args);
		void setCgiExtension(std::string extension_args);
		void setUri(std::string uri_args);
		void setAlias(std::string alias_path);
		void setAllowedMethods(unsigned char allowed_method);
		void setRoot(std::string root_args);
		void setIndex(std::string index_args);
		void setAutoIndex(bool value);
		void setClientMaxBodySize(unsigned int body_size_value);
};

#endif
