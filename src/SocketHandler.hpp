#ifndef SOCKETHANDLER_HPP
# define SOCKETHANDLER_HPP
# include <netinet/in.h> // sockaddr_in

class SocketHandler
{
	public:
		static int create_socket();
		static void set_socket_options(int sock);
		static void bind_socket(int sock, struct sockaddr_in &address, int port);
		static void listen_socket(int sock, int backlog);
		static int accept_connection(int server_sock, struct sockaddr_in &address, socklen_t &addrlen);
		static void set_nonblocking(int sock);
};

#endif
