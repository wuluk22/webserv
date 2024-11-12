#include "HttpRequestHandler.hpp"
#include "ServerHandler.hpp"
#include <cerrno>

const int BUFFER_SIZE = 1024;

int HttpRequestHandler::handle_request(int client_sock)
{
	ServerHandler Server;

    char buffer[BUFFER_SIZE];
    int valread = recv(client_sock, buffer, BUFFER_SIZE, 0);

    if (valread == 0)  // Client disconnected
    {
        std::cout << "Client disconnected, closing socket" << std::endl;
        return 1;
    }
    else if (valread < 0)
    {
        std::cerr << "Error in receiving data : " << strerror(errno) << std::endl; // Warning, do not use Errno
        return 3; // TO DO : handle the case of error in receiving datas
    }

    buffer[valread] = '\0';
    std::cout << "Received request: " << buffer << std::endl;
	
	// close(client_sock); // Close the client socket after sending response
	return 0;
}

