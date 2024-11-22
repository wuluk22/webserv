#ifndef SERVERBASE_HPP
#define SERVERBASE_HPP

#include "ServerHandler.hpp"
#include "ErrorHandler.hpp"
#include "RequestResponseState.hpp"

#include <iostream>
#include <sys/select.h>
// #include <netdb.h>
#include <vector>
#include <map>

class	ServerBase
{
	private:
		int							max_sock;
		std::map<int, RRState>		ClientSockets;
		std::vector<ServerHandler>	Servers;
		fd_set 						readfds, writefds;

	public:
		//METHODS
		ServerBase();
		~ServerBase();
		fd_set&						get_readfds();
		fd_set&						get_writefds();
		std::vector<ServerHandler>	get_servers();


		void						processClientConnections();
		void						accept_connection(ServerHandler	Server);
		void						addPortAndServers();
};

#endif
