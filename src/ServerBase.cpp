
#include "ServerBase.hpp"
#include "ServerHandler.hpp"
#include "RequestResponseState.hpp"
#include "configuration_file_parsing/ServerConfig.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

//METHODS
ServerBase::ServerBase() : max_sock(0) {
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

ServerBase::~ServerBase() {
	for (std::vector<ServerHandler>::iterator it = Servers.begin(); it != Servers.end(); it++) {
	int sock = it->getSock();
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
void	ServerBase::addPortAndServers(std::map <size_t, ServerConfig *> AllServersConfig)
{
	for (std::map<size_t, ServerConfig *>::iterator it = AllServersConfig.begin(); it != AllServersConfig.end(); it++) {
		s_server_params server_params = it->second->getServerHeader()->getServerParams();
		std::vector<LocationBlock *> directives = it->second->getDirectives();
		for (std::set<unsigned int>::iterator it = server_params._listen.begin(); it != server_params._listen.end(); it++) {
			ServerHandler NewServer;
			NewServer.InitializeServerSocket(*it, 3);
			NewServer.setLocations(directives);
			std::cout << "NewServer -> " << "FD : " << NewServer.getSock() << "|| PORT : " << NewServer.getPort() << std::endl;
			FD_SET(NewServer.getSock(), &get_readfds());
			this->max_sock = std::max(this->max_sock, NewServer.getSock());
			Servers.push_back(NewServer);
		}
	}
}

void	ServerBase::accept_connection(ServerHandler	Server)
{
    int new_socket = accept(Server.getSock(), Server.getAddress(), &Server.getAddrlen());
    if (new_socket < 0) {
		throw ServerBaseError("Accept failed", __FUNCTION__, __LINE__);
	}
	std::cout << "New accepted connection : " << new_socket << std::endl;
	FD_SET(new_socket, &readfds);
	// if (ClientSockets.find(new_socket) == ClientSockets.end()) {
		RRState NewConnectionState;
		NewConnectionState.setLocations(Server.getLocations());
		// for (std::vector<LocationBlock *>::iterator it = NewConnectionState.getLocations().begin(); it != NewConnectionState.getLocations().end(); it++) {
		// 	std::cout << "SOCKETPART : ";
		// 	std::cout << *it << std::endl;
		// }
        ClientSockets.insert(std::make_pair(new_socket, NewConnectionState));
	// }
	if (new_socket > max_sock)
		max_sock = new_socket;
	// std::cout << "max_sock : " << max_sock << std::endl;
}

void	ServerBase::processClientConnections()
{
	fd_set	cpyReadFds, cpyWriteFds;
	// struct timeval timeout;

	// timeout.tv_sec = 30;
	while (true)
    {
		cpyReadFds = readfds;
		cpyWriteFds = writefds;
		std::vector<int> clientToRemove;

		// std::cout << "BEGIN PROGRAM" << std::endl;
		// print_fd_set(readfds, "readfds");
		// print_fd_set(writefds, "writefds");
		// print_fd_set(cpyReadFds, "cpyReadFds");
		// print_fd_set(cpyWriteFds, "cpyWriteFds");
		std::cout << std::endl;
        // Wait for an activity on one of the sockets
        if (select(max_sock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0) {
			// print_fd_set(cpyReadFds, "IN SELECT cpyReadFds");
			// print_fd_set(cpyWriteFds, "cpyWriteFds");
			throw ServerBaseError("Select failed", __FUNCTION__, __LINE__);
		}
        // If activity on server socket, it's an incoming connection
		for (unsigned long i = 0; i < this->Servers.size(); i++) {
			int serverSocket = this->Servers[i].getSock();
			if (FD_ISSET(serverSocket, &cpyReadFds)) {
				accept_connection(this->Servers[(int)i]);
			}
		}
		// Handling Request/Response
		for (std::map<int, RRState>::iterator it = ClientSockets.begin(); it != ClientSockets.end(); it++) {
			int client_sock = it->first;
			std::vector<LocationBlock *> loc = it->second.getLocations();
			if (FD_ISSET(client_sock, &cpyReadFds)) {
				HttpRequestHandler request = it->second.getRequest();
				request = request.handleRequest(client_sock, loc);
				//request = it->second.initRequest(request);
				it->second.setRequest(request);
				// std::cout << "Request reponse : " << request.getFd() << std::endl;
				std::cerr << "process 2-\n" << request << "end!" << std::endl;
				if (request.getFd() <= 0 ) { // Client Disconnected
					close(client_sock);
					FD_CLR(client_sock, &readfds);
					FD_CLR(client_sock, &writefds);
					clientToRemove.push_back(client_sock);
					continue ;
				}
				if (request.getIsComplete() == true) {
					FD_CLR(client_sock, &readfds);
					FD_SET(client_sock, &cpyWriteFds);
				}
			}
			if (FD_ISSET(client_sock, &cpyWriteFds))
            {
				HttpResponseHandler response = it->second.getResponse();
                response.handleResponse(it->second.getRequest(), client_sock);
				std::cerr << "process 3-\n" << it->second.getRequest() << "end!" << std::endl;
				it->second.setResponse(response);
				close(client_sock);
				FD_CLR(client_sock, &writefds);
            }
		}
		for(unsigned long i = 0; i < clientToRemove.size(); i++) {
			for (std::map<int, RRState>::iterator it = ClientSockets.begin(); it != ClientSockets.end();) {
				if (it->first == clientToRemove[(int)i]) {
					std::map<int, RRState>::iterator toErase = it;
					++it;
					ClientSockets.erase(toErase);
				}
				else {
				 	it++;
				}
			}
		}
		// std::cout << "END OF PROGRAM" << std::endl;
		// print_fd_set(cpyReadFds, "cpyReadFds");
		// print_fd_set(cpyWriteFds, "cpyWriteFds");
		// to verify the content of clientSockets
		// for (unsigned long i = 0; i < ClientSockets.size(); i++) {
		// 	std::cout << i << " : " << ClientSockets[i] << std::endl;
		// }
    }
}
