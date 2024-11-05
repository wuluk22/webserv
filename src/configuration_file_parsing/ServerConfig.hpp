#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>
#include <iostream>

class Directive;

class ServerConfig {
	private:
		std::vector<Directive> all_directives;

	public:
		ServerConfig(void);
		ServerConfig(const ServerConfig &copy);
		~ServerConfig();
		ServerConfig& operator=(const ServerConfig& rhs);
		std::vector<Directive> getAllDirectives(void) const;
		void addOneDirective(Directive element_to_add);
};

class Directive {
	public:
		Directive(void);
		Directive(const ServerConfig &copy);
		~Directive();
		Directive& operator = (const Directive &rhs);
};

struct s_common_parameters {
	std::string _root;
	std::string _index;
	bool auto_index;
};

struct s_server_block_parameters {
	std::string _server_name;
	std::vector<unsigned int> _listen;
	unsigned int _client_max_body_size;
};

class ServerBlock : public Directive {
	private:
		s_common_parameters _common_params;
		s_server_block_parameters _server_params;
	public:
		ServerBlock(s_common_parameters common_params, s_server_block_parameters server_params);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);
		void getCommonParams(void) const;
		void getServerParams(void) const;
		void setCommonParams(s_common_parameters common_params);
		void setServerParams(s_server_block_parameters server_params);

};

enum e_allowed_methods {
	GET = 1 << 0,  //0001
	POST = 1 << 1,  //0010
	DELETE = 1 << 2   //0100
};

struct s_location_block_parameters {
	std::string _uri;
	std::string _alias;
	unsigned char _allowed_methods;
};

class LocationBlock : public Directive {
	private:
		s_common_parameters _common_params;
		s_location_block_parameters _location_params;
	public:
		LocationBlock(s_common_parameters common_params, s_location_block_parameters location_params);
		LocationBlock(const LocationBlock &copy);
		~LocationBlock();
		LocationBlock& operator = (const LocationBlock &rhs);
		void getCommonParams(void) const;
		void getLocationParams(void) const;
		void setCommonParams(s_common_parameters common_params);
		void setLocationParams(s_location_block_parameters location_params);
};

#endif