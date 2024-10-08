#include "SocketHandler.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

int SocketHandler::create_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return sock;
}

void SocketHandler::set_socket_options(int sock)
{
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
    {
        std::cerr << "setsockopt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SocketHandler::bind_socket(int sock, struct sockaddr_in &address, int port)
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void SocketHandler::listen_socket(int sock, int backlog)
{
    if (listen(sock, backlog) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int SocketHandler::accept_connection(int server_sock, struct sockaddr_in &address, socklen_t &addrlen)
{
    int new_socket = accept(server_sock, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

void SocketHandler::set_nonblocking(int sock)
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

