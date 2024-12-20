#ifndef SOCKETHANDLER_HPP
# define SOCKETHANDLER_HPP

#include "configuration_file_parsing/server_config/ServerConfig.hpp"

#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

class ErrorHandler;

class ServerHandler
{
	private:
		struct sockaddr_in				_address;
		socklen_t						_addrlen;
		int								_sock;
		int								_port;
		std::vector<LocationBlock *>	_locations;

	public:
		// METHODS
		ServerHandler();
		~ServerHandler();
		int&							getSock();
		int&							getPort();
		struct sockaddr*				getAddress();
		struct sockaddr_in				getAdd();
		socklen_t&						getAddrlen();
		std::vector<LocationBlock *>&	getLocations();

		void				setLocations(std::vector<LocationBlock *>& locations);
		void				InitializeServerSocket(int port, const int backlog);
		int					createSocket();
		void				setSocketOptions(int sock);
		void				bindSocket(int port);
		void				listenSocket(int sock, int backlog);
		void				setNonblocking(int sock);
};

void						printFdSet(const fd_set& fdset, std::string functionName);

#endif