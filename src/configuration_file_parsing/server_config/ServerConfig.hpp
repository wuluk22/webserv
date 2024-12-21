#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "ServerBlock.hpp"
#include "LocationBlock.hpp"
#include "../../Logger.hpp"

class ServerConfig {
	private:
		std::vector<LocationBlock *>	_directives;
		ServerBlock						*_server_header;
		Logger							_logger;
	public:
		ServerConfig(void);
		~ServerConfig();
		std::vector<LocationBlock *>	getDirectives(void) const;
		void							setDirective(LocationBlock *new_directive);
		ServerBlock*					getServerHeader(void) const;
		void							setServerHeader(ServerBlock *server_header);
		void							setLogger(void);
		Logger 							getLogger(void);
};

#endif