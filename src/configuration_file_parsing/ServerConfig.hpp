#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>
#include "ConfigException.hpp"

class Directive;

struct s_common_params {
	std::string _root;
	std::string _index;
	bool auto_index;
};

struct s_server_params {
	std::string _server_name;
	std::vector<unsigned int> _listen;
	unsigned int _client_max_body_size;
};

struct s_loc_params {
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
		std::vector<Directive> _all_directives;
		bool _server_params_defined;
	public:
		ServerConfig(void);
		ServerConfig(const ServerConfig &copy);
		~ServerConfig();
		ServerConfig& operator=(const ServerConfig& rhs);
		std::vector<Directive> getAllDirectives(void) const;
		void setDirective(Directive new_directive);
		void addLocationDirective(s_common_params c_params, s_loc_params l_params);
		void addServerDirective(s_common_params c_params, s_server_params s_params);
};

// Directive class sole purpose is to enable a composite design pattern for the ServerConfig class

class Directive {
	public:
		Directive(void);
		virtual ~Directive();
		virtual s_common_params getCommonParams(void) const;
		virtual void setCommonParams(s_server_params s_params);
};


class ServerBlock : public Directive {
	private:
		s_common_params _common_params;
		s_server_params _server_params;
	public:
		ServerBlock(s_common_params common_params, s_server_params server_params);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);
		s_common_params getCommonParams(void) const;
		s_server_params getServerParams(void) const;
		void setCommonParams(s_common_params c_params);
		void setServerParams(s_server_params s_params);
};

class LocationBlock : public Directive {
	private:
		s_common_params _common_params;
		s_loc_params _location_params;
	public:
		LocationBlock(s_common_params common_params, s_loc_params location_params);
		LocationBlock(const LocationBlock &copy);
		~LocationBlock();
		LocationBlock& operator = (const LocationBlock &rhs);
		s_common_params getCommonParams(void) const;
		s_loc_params getLocationParams(void) const;
		void setCommonParams(s_common_params common_params);
		void setLocationParams(s_loc_params location_params);
};

#endif