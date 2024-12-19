#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "ServerBlock.hpp"
#include "LocationBlock.hpp"

class ServerConfig {
	private:
		std::vector<LocationBlock *> directives;
		ServerBlock *server_header;
	public:
		ServerConfig(void);
		~ServerConfig();
		std::vector<LocationBlock *>	getDirectives(void) const;
		void							setDirective(LocationBlock *new_directive);
		ServerBlock*					getServerHeader(void) const;
		void							setServerHeader(ServerBlock *server_header);
};

#endif