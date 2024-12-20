#ifndef SERVERBASE_HPP
#define SERVERBASE_HPP

#include "RequestResponseState.hpp"
#include "ServerHandler.hpp"
#include "configuration_file_parsing/server_config/ServerConfig.hpp"

#include <iostream>
#include <sys/select.h>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

class	ServerBase
{
	private:
		int							maxSock;
		std::map<int, RRState>		ClientSockets;
		std::vector<ServerHandler>	Servers;
		fd_set 						readfds, writefds;

	public:
		//METHODS
		ServerBase();
		~ServerBase();
		fd_set&						getReadfds();
		fd_set&						getWritefds();
		std::vector<ServerHandler>	getServers();


		void						processClientConnections();
		void						acceptConnection(ServerHandler	Server);
		void						addPortAndServers(std::map <size_t, ServerConfig *> AllServersConfig);
};

#endif
