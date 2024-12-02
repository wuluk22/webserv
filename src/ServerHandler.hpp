#ifndef SOCKETHANDLER_HPP
# define SOCKETHANDLER_HPP

#include "ErrorHandler.hpp"
#include "configuration_file_parsing/ServerConfig.hpp"

#include <netinet/in.h>
// #include <sys/_types/_socklen_t.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

class ServerHandler
{
	private:
		// struct addrinfo 	hints; // for getaddrinfo();
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

void print_fd_set(const fd_set& fdset, std::string functionName);

#endif
