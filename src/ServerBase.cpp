
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
		throw ServerBaseError("Accept failed", __FUNCTION__, __LINE__);
	std::cout << "New accepted connection : " << new_socket << std::endl;
	FD_SET(new_socket, &readfds);
	std::cout << "ICI" << std::endl;
	if (ClientSockets.find(new_socket) == ClientSockets.end()) {
		ConnectionState NewConnectionState;
        ClientSockets.insert(std::make_pair(new_socket, NewConnectionState)); // ajouter un nouvel état de connexion
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
        // Wait for an activity on one of the sockets
        if (select(max_sock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0)
			throw ServerBaseError("Select failed", __FUNCTION__, __LINE__);

        // If activity on server socket, it's an incoming connection
		for (unsigned long i = 0; i < this->Servers.size(); i++) {
			int serverSocket = this->Servers[i].get_sock();
			if (FD_ISSET(serverSocket, &cpyReadFds)) {
				std::cout << "serverSocket" << serverSocket << std::endl;
				accept_connection(this->Servers[(int)i]);
			}
		}

		// Handling Request/Response
		for (std::map<int, ConnectionState>::iterator it = ClientSockets.begin(); it != ClientSockets.end(); it++) {
			int client_sock = it->first;
			std::cout << "it->first : " << it->first << std::endl;
			if (FD_ISSET(client_sock, &cpyReadFds)) {
				int resultRequest = HttpRequestHandler::handle_request(client_sock);
				if (resultRequest == 1 || resultRequest == 3) { // Client Disconnected
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
                HttpResponseHandler::handle_response(client_sock);
				// close(client_sock);
				FD_CLR(client_sock, &writefds);
            }
		}
		for(unsigned long i = 0; i < clientToRemove.size(); i++) {
			for (std::map<int, ConnectionState>::iterator it = ClientSockets.begin(); it != ClientSockets.end();) {
				if (it->first == clientToRemove[(int)i])
					it = ClientSockets.erase(it);
				else
				 	it++;
			}
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
