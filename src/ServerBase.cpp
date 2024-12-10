
#include "ServerBase.hpp"
#include "ServerHandler.hpp"
#include "RequestResponseState.hpp"
#include "configuration_file_parsing/ServerConfig.hpp"

#include <algorithm>
#include <cstring>
#include <vector>

//METHODS
ServerBase::ServerBase() : maxSock(0)
{
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

ServerBase::~ServerBase()
{
	for (std::vector<ServerHandler>::iterator it = Servers.begin(); it != Servers.end(); it++)
	{
		int sock = it->getSock();
		FD_CLR(sock, &readfds);
		FD_CLR(sock, &writefds);
		close(sock);
	}
}

/////////// GETTER ///////////////
fd_set&	ServerBase::getReadfds() { return readfds; }
fd_set&	ServerBase::getWritefds() { return writefds; }
std::vector<ServerHandler>	ServerBase::getServers() { return Servers; }

////////// PUBLIC /////////////
void	ServerBase::addPortAndServers(std::map <size_t, ServerConfig *> AllServersConfig)
{
	for (std::map<size_t, ServerConfig *>::iterator it = AllServersConfig.begin(); it != AllServersConfig.end(); it++)
	{
		s_server_params server_params = it->second->getServerHeader()->getServerParams();
		std::vector<LocationBlock *> directives = it->second->getDirectives();
		for (std::set<unsigned int>::iterator it = server_params._listen.begin(); it != server_params._listen.end(); it++)
		{
			ServerHandler NewServer;
			NewServer.InitializeServerSocket(*it, 3);
			NewServer.setLocations(directives);
			std::cout << "NewServer -> " << "FD : " << NewServer.getSock() << "|| PORT : " << NewServer.getPort() << std::endl;
			FD_SET(NewServer.getSock(), &getReadfds());
			this->maxSock = std::max(this->maxSock, NewServer.getSock());
			Servers.push_back(NewServer);
		}
	}
}

void	ServerBase::acceptConnection(ServerHandler	Server)
{
    int newSocket = accept(Server.getSock(), Server.getAddress(), &Server.getAddrlen());
    if (newSocket < 0)
	{
		throw ServerBaseError("Accept failed", __FUNCTION__, __LINE__);
	}
	std::cout << "New accepted connection : " << newSocket << std::endl;
	FD_SET(newSocket, &readfds);
	// if (ClientSockets.find(newSocket) == ClientSockets.end()) {
		RRState NewConnectionState;
		NewConnectionState.setLocations(Server.getLocations());
		// for (std::vector<LocationBlock *>::iterator it = NewConnectionState.getLocations().begin(); it != NewConnectionState.getLocations().end(); it++) {
		// 	std::cout << "SOCKETPART : ";
		// 	std::cout << *it << std::endl;
		// }
        ClientSockets.insert(std::make_pair(newSocket, NewConnectionState));
	// }
	if (newSocket > maxSock)
		maxSock = newSocket;
	// std::cout << "maxSock : " << maxSock << std::endl;
}

void	ServerBase::processClientConnections()
{
	fd_set	cpyReadFds, cpyWriteFds;
	// struct timeval timeout;

	// timeout.tv_sec = 3;
	while (true)
    {
		cpyReadFds = readfds;
		cpyWriteFds = writefds;
		std::vector<int> clientToRemove;

        // Wait for an activity on one of the sockets
        if (select(maxSock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0)
		{
			throw ServerBaseError("Select failed", __FUNCTION__, __LINE__);
		}
        // If activity on server socket, it's an incoming connection
		for (unsigned long i = 0; i < this->Servers.size(); i++)
		{
			int serverSocket = this->Servers[i].getSock();
			std::cout << "serverSocket : " << serverSocket << std::endl;
			if (FD_ISSET(serverSocket, &cpyReadFds))
			{
				acceptConnection(this->Servers[(int)i]);
			}
		}
		// Handling Request/Response
		for (std::map<int, RRState>::iterator it = ClientSockets.begin(); it != ClientSockets.end(); it++)
		{
			int client_sock = it->first;
			std::cout << "client sock : " << client_sock << std::endl;
			std::vector<LocationBlock *> *loc = &it->second.getLocations();
			if (FD_ISSET(client_sock, &cpyReadFds))
			{
				HttpRequestHandler request = it->second.getRequest();
				request = request.handleRequest(client_sock, loc);
				//request = it->second.initRequest(request);
				it->second.setRequest(request);
				// std::cout << "Request reponse : " << request.getFd() << std::endl;
				//std::cerr << "process 2-\n" << request << "end!" << std::endl;
				if (request.getFd() <= 0)
				{ 
					// Client Disconnected
					close(client_sock);
					FD_CLR(client_sock, &readfds);
					FD_CLR(client_sock, &writefds);
					clientToRemove.push_back(client_sock);
					continue ;
				}
				if (request.getIsComplete() == true)
				{
					FD_CLR(client_sock, &readfds);
					FD_SET(client_sock, &cpyWriteFds);
				}
			}
			if (FD_ISSET(client_sock, &cpyWriteFds))
            {
				HttpResponseHandler response = it->second.getResponse();
                response.handleResponse(it->second.getRequest(), client_sock);
				//std::cerr << "process 3-\n" << it->second.getRequest() << "end!" << std::endl;
				it->second.setResponse(response);
				close(client_sock);
				FD_CLR(client_sock, &writefds);
            }
		}
		for(unsigned long i = 0; i < clientToRemove.size(); i++)
		{
			for (std::map<int, RRState>::iterator it = ClientSockets.begin(); it != ClientSockets.end();)
			{
				if (it->first == clientToRemove[(int)i])
				{
					std::map<int, RRState>::iterator toErase = it;
					++it;
					ClientSockets.erase(toErase);
				}
				else
				{
				 	it++;
				}
			}
		}
    }
}
