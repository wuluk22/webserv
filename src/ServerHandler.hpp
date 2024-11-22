#ifndef SOCKETHANDLER_HPP
# define SOCKETHANDLER_HPP

#include "ErrorHandler.hpp"

#include <netinet/in.h>
// #include <sys/_types/_socklen_t.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

class ServerHandler
{
	private:
		// struct addrinfo 	hints; // for getaddrinfo();
		struct sockaddr_in	_address;
		socklen_t			_addrlen;
		int					_sock;
		int					_port;
		std::string			_serverName;

	public:
		// METHODS
		ServerHandler();
		~ServerHandler();
		int&				getSock();
		int&				getPort();
		struct sockaddr*	getAddress();
		socklen_t&			getAddrlen();

		void				InitializeServerSocket(int port, const int backlog);
		int					createSocket();
		void				setSocketOptions(int sock);
		void				bindSocket(int port);
		void				listenSocket(int sock, int backlog);
		void				setNonblocking(int sock);
};

void print_fd_set(const fd_set& fdset, std::string functionName);

#endif
