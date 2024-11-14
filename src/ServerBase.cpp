
#include "ServerBase.hpp"
#include "ServerHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "HttpResponseHandler.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <cstring>

//METHODS
ServerBase::ServerBase() : max_sock(0) {
	// Initialize the socket set to zero
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

ServerBase::~ServerBase() {

}

/////////// GETTER ///////////////
fd_set&	ServerBase::get_readfds() {
	return readfds;
}

fd_set&	ServerBase::get_writefds() {
	return writefds;
}

std::vector<ServerHandler>	ServerBase::get_servers() {
	return Servers;
}

////////// PUBLIC /////////////
void	ServerBase::addPortAndServers()
{
	std::vector<int>	ports;
	ports.push_back(8080);
	ports.push_back(4242);
	ports.push_back(10);
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++) {
		ServerHandler NewServer;
		NewServer.InitializeServerSocket(*it, 3);
		FD_SET(NewServer.get_sock(), &get_readfds());
		this->max_sock = std::max(this->max_sock, NewServer.get_sock());
		Servers.push_back(NewServer);
	}
}

void	ServerBase::accept_connection(ServerHandler	Server)
{
    int new_socket = accept(Server.get_sock(), Server.get_address(), &Server.get_addrlen());
    if (new_socket < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        exit(EXIT_FAILURE);
    }
	std::cout << "New accepted connection : " << new_socket << std::endl;
	FD_SET(new_socket, &readfds);
	if (std::find(clientSockets.begin(), clientSockets.end(), new_socket) == clientSockets.end()) {
		clientSockets.push_back(new_socket); // don't forget to delete the clientfd when it's not used anymore
	}
	if (new_socket > max_sock)
		max_sock = new_socket;
	std::cout << "max_sock : " << max_sock << std::endl;
}

void	ServerBase::processClientConnections()
{
	fd_set	cpyReadFds, cpyWriteFds;
	while (true)
    {
		cpyReadFds = readfds;
		cpyWriteFds = writefds;

        // Wait for an activity on one of the sockets
		// print_fd_set(readfds);
        if (select(max_sock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0) { // warning :: i cannot use errno to verify errors
            Logger::log_error("Select error");
			if (errno == EINTR) {
			// Signal interruption, continue loop
			continue;
			} else if (errno == EBADF) {
				perror("Select failed due to an invalid file descriptor (EBADF)");
				// Additional code to check validity of each fd if necessary
			} else if (errno == ENOMEM) {
				perror("Select failed due to insufficient memory (ENOMEM)");
				break;  // Exiting might be safer here
			} else if (errno == EINVAL) {
				perror("Select failed due to invalid timeout or max_sd (EINVAL)");
				break;  // Potentially a serious configuration issue
			} else {
				perror("Select failed for an unknown reason");
				break;
			}
		}

        // If activity on server socket, it's an incoming connection
		for (unsigned long i = 0; i < this->Servers.size(); i++) {
			int serverSocket = this->Servers[i].get_sock();
			if (FD_ISSET(serverSocket, &cpyReadFds)) {
				// if (serverSocket == serverSockets[0])
				accept_connection(this->Servers[(int)i]);
			}
		}

		// Handling Request/Response
		for (std::vector<int>::iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
			int client_sock = *it;
			if (FD_ISSET(client_sock, &cpyReadFds)) {
				int resultRequest = HttpRequestHandler::handle_request(client_sock);
				if (resultRequest == 1 || resultRequest == 3) { // Client Disconnected
					std::cout << "TESTING ERROR CLIENT DISCONNECTED" << std::endl;
					close(client_sock);
					FD_CLR(client_sock, &readfds);
					clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client_sock));
					if (clientSockets.size() == 0)
						break ;
					continue ;
				}
				FD_CLR(client_sock, &readfds);
				FD_SET(client_sock, &cpyWriteFds); // Warning maybe it can cause problems
			}
			if (FD_ISSET(client_sock, &cpyWriteFds))
            {
                HttpResponseHandler::handle_response(clientSockets.back());
				close(client_sock); // if i close it here, don't close in HttpRequestHandler
				FD_CLR(client_sock, &writefds);
				// clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), client_sock));
            }
		}
		// to verify the content of clientSockets
		// for (unsigned long i = 0; i < clientSockets.size(); i++) {
		// 	std::cout << i << " : " << clientSockets[i] << std::endl;
		// }
    }
}