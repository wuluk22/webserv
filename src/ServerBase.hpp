#ifndef SERVERBASE_HPP
#define SERVERBASE_HPP

#include "ServerHandler.hpp"
#include "ErrorHandler.hpp"
#include "RequestResponseState.hpp"

#include <iostream>
#include <sys/select.h>
#include <vector>
#include <map>

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
