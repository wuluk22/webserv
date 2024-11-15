#include "ServerHandler.hpp"
#include "ErrorHandler.hpp"
#include "Logger.hpp"
#include <sys/_types/_fd_def.h>

// DEBUG // DON'T FORGET TO DELETE
void print_fd_set(const fd_set& fdset, std::string functionName) {
    std::cout << "Current fds in " << functionName << " : ";
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, &fdset)) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
}

// METHODS //
ServerHandler::ServerHandler() : addrlen(sizeof(address)){}
ServerHandler::~ServerHandler() {}

// GETTER //
int&	ServerHandler::get_sock() { return sock; }
struct sockaddr* ServerHandler::get_address() { return reinterpret_cast<struct sockaddr*>(&address); }
socklen_t&	ServerHandler::get_addrlen() { return addrlen; }


void ServerHandler::InitializeServerSocket(int port, const int backlog)
{
	this->port = port;
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
		throw ServerHandlerError("Creation of socket failed", __FUNCTION__, __LINE__);
    return sock;
}

void ServerHandler::set_socket_options(int sock)
{
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		throw ServerHandlerError("setsockopt failed with SO_REUSEADDR", __FUNCTION__, __LINE__);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, sizeof(opt)) < 0)
		throw ServerHandlerError("setsockopt failed with SO_REUSEPORT", __FUNCTION__, __LINE__);
}

void ServerHandler::bind_socket(int port)
{
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(port);

	// std::cout << "address.sin_addr.s_addr : " << address.sin_addr.s_addr << std::endl;
	// std::cout << "address.sin_port :" << address.sin_port << std::endl;
	if (bind(this->sock, (struct sockaddr*)&address, sizeof(address)) < 0)
		throw ServerHandlerError("bind failed", __FUNCTION__, __LINE__);
}

void ServerHandler::listen_socket(int sock, int backlog)
{
    if (listen(sock, backlog) < 0)
		throw ServerHandlerError("listen failed", __FUNCTION__, __LINE__);
}

void ServerHandler::set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
		throw ServerHandlerError("fcntl failed : unable to get flags", __FUNCTION__, __LINE__);
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
		throw ServerHandlerError("fcntl failed: unable to set non-blocking", __FUNCTION__, __LINE__);
}
