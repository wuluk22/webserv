#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <vector>

class ServerConfig {
	private:
		std::vector<Directive> all_directives;
	public:
		ServerConfig(void);
		ServerConfig(const ServerConfig &copy);
		~ServerConfig();
		ServerConfig& operator=(const ServerConfig& rhs);
		std::vector<Directive> getAllDirectives(void) const;
};

class Directive {
	public:
		Directive(void);
		Directive(const ServerConfig &copy);
		~Directive();
		Directive& operator = (const Directive &rhs);
};

class ServerBlock : public Directive {
	private:
		std::string server_name;
		std::vector<unsigned int> listen;
		std::string root;
		std::string index;
		bool auto_index;
		unsigned int client_max_body_size;
	public:
		ServerBlock(void);
		ServerBlock(const ServerBlock &copy);
		~ServerBlock();
		ServerBlock& operator = (const ServerBlock &rhs);

};

enum e_allowed_methods {
    GET    = 1 << 0,  //0001
    POST   = 1 << 1,  //0010
    DELETE = 1 << 2   //0100
};

class LocationBlock : public Directive {
	private:
		std::string alias;
		std::string root;
		std::string index;
		bool auto_index;
		unsigned int client_max_body_size;
		unsigned char allowed_methods;
	public:
		LocationBlock(void);
		LocationBlock(const LocationBlock &copy);
		~LocationBlock();
		LocationBlock& operator = (const LocationBlock &rhs);
};

#endif