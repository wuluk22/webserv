#include "ServerHandler.hpp"
#include "Logger.hpp"
#include <sys/_types/_fd_def.h>

// DEBUG //
void print_fd_set(const fd_set& fdset) {
    std::cout << "Current fds in readfds: ";
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &fdset)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

// METHODS
ServerHandler::ServerHandler() : addrlen(sizeof(address)){
}

ServerHandler::~ServerHandler() {} // delete socket client and server

int&	ServerHandler::get_sock() {
	return sock;
}

struct sockaddr* ServerHandler::get_address() {
    return reinterpret_cast<struct sockaddr*>(&address);
}


socklen_t&	ServerHandler::get_addrlen() {
	return addrlen;
}

void ServerHandler::InitializeServerSocket(int port, const int backlog)
{
	this->port = port;
    // Create server socket
    this->sock = create_socket();

    // Set server socket to reuse address
    set_socket_options(this->sock);

    // Bind the server socket to a port
    bind_socket(port);

    // Set server socket to non-blocking
    set_nonblocking(this->sock);

    // Start listening for connections
    listen_socket(this->sock, backlog);
    Logger::log("Server listening for connections");
}

int ServerHandler::create_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

void ServerHandler::set_socket_options(int sock)
{
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsockopt failed with SO_REUSEADDR" << std::endl;
        exit(EXIT_FAILURE);
    }
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsockopt failed with SO_REUSEPORT" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ServerHandler::bind_socket(int port)
{
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(port);

	std::cout << "address.sin_addr.s_addr : " << address.sin_addr.s_addr << std::endl;
	std::cout << "address.sin_port :" << address.sin_port << std::endl;
	if (bind(this->sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		std::cerr << "Bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ServerHandler::listen_socket(int sock, int backlog)
{
    if (listen(sock, backlog) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ServerHandler::set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error in fcntl: unable to get flags" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error in fcntl: unable to set non-blocking" << std::endl;
        exit(EXIT_FAILURE);
    }
}
