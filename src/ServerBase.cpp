
#include "ServerBase.hpp"
#include "ServerHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "HttpResponseHandler.hpp"
#include <algorithm>
#include <cstring>

//METHODS
ServerBase::ServerBase() : max_sock(0) {
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

ServerBase::~ServerBase() {
	for (std::vector<ServerHandler>::iterator it = Servers.begin(); it != Servers.end(); it++) {
		int sock = it->get_sock();
		FD_CLR(sock, &readfds);
		FD_CLR(sock, &writefds);
		close(sock);
	}
}

/////////// GETTER ///////////////
fd_set&	ServerBase::get_readfds() { return readfds; }
fd_set&	ServerBase::get_writefds() { return writefds; }
std::vector<ServerHandler>	ServerBase::get_servers() { return Servers; }

////////// PUBLIC /////////////
void	ServerBase::addPortAndServers()
{
	std::vector<int>	ports;
	ports.push_back(8080);
	ports.push_back(4242);
	ports.push_back(1025);
	for (std::vector<int>::iterator it = ports.begin(); it != ports.end(); it++) {
		ServerHandler NewServer;
		NewServer.InitializeServerSocket(*it, 3);
		// std::cout << "NewServer: " << NewServer.get_port() << std::endl;
		FD_SET(NewServer.get_sock(), &get_readfds());
		this->max_sock = std::max(this->max_sock, NewServer.get_sock());
		Servers.push_back(NewServer);
	}
	// getaddrinfo(¨http://coucou.be¨, ¨8080¨, )
}

void	ServerBase::accept_connection(ServerHandler	Server)
{
    int new_socket = accept(Server.get_sock(), Server.get_address(), &Server.get_addrlen());
    if (new_socket < 0) {
		throw ServerBaseError("Accept failed", __FUNCTION__, __LINE__);
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
		std::vector<int> clientToRemove;

		std::cout << "BEGIN PROGRAM" << std::endl;
		// print_fd_set(readfds, "readfds");
		// print_fd_set(writefds, "writefds");
		// print_fd_set(cpyReadFds, "cpyReadFds");
		// print_fd_set(cpyWriteFds, "cpyWriteFds");
		std::cout << std::endl;
        // Wait for an activity on one of the sockets
        if (select(max_sock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0) {
			print_fd_set(cpyReadFds, "IN SELECT cpyReadFds");
			print_fd_set(cpyWriteFds, "cpyWriteFds");
			throw ServerBaseError("Select failed", __FUNCTION__, __LINE__);
		}

        // If activity on server socket, it's an incoming connection
		for (unsigned long i = 0; i < this->Servers.size(); i++) {
			int serverSocket = this->Servers[i].get_sock();
			if (FD_ISSET(serverSocket, &cpyReadFds)) {
				accept_connection(this->Servers[(int)i]);
			}
		}

		// Handling Request/Response
		for (std::vector<int>::iterator it = clientSockets.begin(); it != clientSockets.end(); it++) {
			int client_sock = *it;
			HttpRequestHandler	httpRequest;
			HttpResponseHandler	httpResponse;
			if (FD_ISSET(client_sock, &cpyReadFds)) {
				it->second->_request = httpRequest.handleRequest(client_sock); // return class request
				std::cout << "RESULTEQUEST : " << resultRequest << std::endl;
				if (resultRequest <= 0 ) { // Client Disconnected
					close(client_sock);
					FD_CLR(client_sock, &readfds);
					FD_CLR(client_sock, &writefds);
					clientToRemove.push_back(client_sock);
					continue ;
				}
				// FD_CLR(client_sock, &readfds);
				FD_SET(client_sock, &cpyWriteFds);
			}
			if (FD_ISSET(client_sock, &cpyWriteFds))
            {
                it->second->_response = httpResponse.handleResponse(client_sock);
				close(client_sock);
				FD_CLR(client_sock, &writefds);
            }
		}
		for(unsigned long i = 0; i < clientToRemove.size(); i++) {
			clientSockets.erase(std::remove(clientSockets.begin(), clientSockets.end(), clientToRemove[(int)i]), clientSockets.end());
		}
		std::cout << "END OF PROGRAM" << std::endl;
		print_fd_set(cpyReadFds, "cpyReadFds");
		print_fd_set(cpyWriteFds, "cpyWriteFds");
		// to verify the content of clientSockets
		// for (unsigned long i = 0; i < clientSockets.size(); i++) {
		// 	std::cout << i << " : " << clientSockets[i] << std::endl;
		// }
    }
}
