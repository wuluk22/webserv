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
		struct sockaddr_in	address;
		socklen_t			addrlen;
		int					sock;
		int					port;

	public:
		// METHODS
		ServerHandler();
		~ServerHandler();
		int&	get_sock();
		int		get_port();
		struct sockaddr* get_address();
		socklen_t&	get_addrlen();

		void	InitializeServerSocket(int port, const int backlog);
		int		create_socket();
		void	set_socket_options(int sock);
		void	bind_socket(int port);
		void	listen_socket(int sock, int backlog);
		void	set_nonblocking(int sock);
};

void print_fd_set(const fd_set& fdset, std::string functionName);

#endif
