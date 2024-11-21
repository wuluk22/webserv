#ifndef SERVERBASE_HPP
#define SERVERBASE_HPP

#include "ServerHandler.hpp"
#include "ErrorHandler.hpp"

#include <iostream>
#include <sys/_types/_fd_def.h>
#include <vector>
#include <map>

struct ConnectionState {
    std::string requestBuffer;
    bool requestComplete;
    std::string responseBuffer;
    size_t bytesSent;

	ConnectionState() : requestBuffer(""), requestComplete(false), responseBuffer(""), bytesSent(0) {}
};

class	ServerBase
{
	private:
		int								max_sock;
		std::map<int, ConnectionState>	ClientSockets;
		std::vector<ServerHandler>		Servers;
		fd_set 							readfds, writefds;

	public:
		//METHODS
		ServerBase();
		~ServerBase();
		fd_set&						get_readfds();
		fd_set&						get_writefds();
		std::vector<ServerHandler>	get_servers();


		void			processClientConnections();
		void			accept_connection(ServerHandler	Server);
		void			addPortAndServers();
};

#endif
