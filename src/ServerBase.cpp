
#include "ServerBase.hpp"
#include "ServerHandler.hpp"
#include "RequestResponseState.hpp"
#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "ErrorHandler.hpp"

//METHODS
ServerBase::ServerBase() : _maxSock(0)
{
	FD_ZERO(&_readfds);
	FD_ZERO(&_writefds);
}

ServerBase::~ServerBase()
{
	for (std::vector<ServerHandler>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		int sock = it->getSock();
		FD_CLR(sock, &_readfds);
		FD_CLR(sock, &_writefds);
		close(sock);
	}
}

/////////// GETTER ///////////////
fd_set&	ServerBase::getReadfds() { return _readfds; }
fd_set&	ServerBase::getWritefds() { return _writefds; }
std::vector<ServerHandler>	ServerBase::getServers() { return _servers; }

////////// PUBLIC /////////////
void	ServerBase::addPortAndServers(std::map <size_t, ServerConfig *> AllServersConfig)
{
	std::string ImagesPath;
	for (std::map<size_t, ServerConfig *>::iterator it = AllServersConfig.begin(); it != AllServersConfig.end(); it++)
	{
		s_server_params server_params = it->second->getServerHeader()->getServerParams();
		std::vector<LocationBlock *> directives = it->second->getDirectives();
		for (std::vector<LocationBlock *>::iterator it = directives.begin(); it != directives.end(); it++) {
			ImagesPath = (*it)->getUriDependance();
			if (!ImagesPath.empty())
				break;
		}
		
		std::string	server_name = it->second->getServerHeader()->getServerName();
		std::map <unsigned int, std::string> error_pages = it->second->getServerHeader()->getErrorPagesRecord();
		for (std::set<unsigned int>::iterator it = server_params._listen.begin(); it != server_params._listen.end(); it++)
		{
			ServerHandler NewServer;
			NewServer.setLocations(directives);
			NewServer.setServerName(server_name);
			NewServer.setImagesPathCgi(ImagesPath);
			NewServer.InitializeServerSocket(*it, 3);
			NewServer.setErrorPages(error_pages);
			_logger.info("Server Created FD: " + toStrInt(NewServer.getSock()) + " ~ Port: " +  toStrInt(NewServer.getPort())); 
			FD_SET(NewServer.getSock(), &getReadfds());
			this->_maxSock = std::max(this->_maxSock, NewServer.getSock());
			_servers.push_back(NewServer);
		}
	}
}

void	ServerBase::acceptConnection(ServerHandler Server)
{
    int newSocket = accept(Server.getSock(), Server.getAddress(), &Server.getAddrlen());
    if (newSocket < 0)
	{
		throw ServerBaseError("Accept failed", __FUNCTION__, __LINE__);
	}
	FD_SET(newSocket, &_readfds);
	RRState NewConnectionState;
	NewConnectionState.setServer(Server);
	_clientSockets.insert(std::make_pair(newSocket, NewConnectionState));
	if (newSocket > _maxSock)
		_maxSock = newSocket;
}

void ServerBase::processClientConnections()
{
    fd_set cpyReadFds, cpyWriteFds;
	HttpRequestHandler	request;

    while (true)
    {
        cpyReadFds = _readfds;
        cpyWriteFds = _writefds;
        std::vector<int> clientToRemove;

        if (select(_maxSock + 1, &cpyReadFds, &cpyWriteFds, NULL, NULL) < 0)
        {
            throw ServerBaseError("Select failed", __FUNCTION__, __LINE__);
        }

        // Accept new connections
        for (unsigned long i = 0; i < this->_servers.size(); i++)
        {
            int serverSocket = this->_servers[i].getSock();
            if (FD_ISSET(serverSocket, &cpyReadFds))
            {
                acceptConnection(this->_servers[(int)i]);
            }
        }

        // Handle client sockets
        for (std::map<int, RRState>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++)
        {
            int client_sock = it->first;

            // Handle read events
            if (FD_ISSET(client_sock, &cpyReadFds))
            {	
				it->second.setClientSock(client_sock);
                request = request.handleRequest(client_sock, it->second);
                it->second.setRequest(request);

                if (request.getFd() <= 0) // Error or closed connection
                {
                    close(client_sock);
                    FD_CLR(client_sock, &_readfds);
                    FD_CLR(client_sock, &_writefds);
                    clientToRemove.push_back(client_sock);
                    continue;
                }

                if (request.getIsComplete() == true)
                {
                    request.reset();
                    FD_CLR(client_sock, &_readfds);
                    FD_SET(client_sock, &cpyWriteFds);
                }
            }

            // Handle write events
            if (FD_ISSET(client_sock, &cpyWriteFds))
            {
                HttpResponseHandler response;
                response = it->second.getResponse();
				response.handleResponse(it->second);
				it->second.setResponse(response);
                it->second.setWriteBuffer(it->second.getResponse().getAll());
                std::string &responseBuffer = it->second.getWriteBuffer();
				std::cout << "RESPONSE :: " << responseBuffer << std::endl;
                if (!responseBuffer.empty())
                {
                    ssize_t bytesSent = send(client_sock, responseBuffer.c_str(), responseBuffer.size(), 0);
                    if (bytesSent <= 0)
                    {
                        close(client_sock);
                        FD_CLR(client_sock, &_writefds);
                        clientToRemove.push_back(client_sock);
                        continue;
                    }
                    responseBuffer.erase(0, bytesSent); // Remove sent bytes
                }

                if (responseBuffer.empty())
                {
                    close(client_sock);
                    FD_CLR(client_sock, &_writefds);
                    clientToRemove.push_back(client_sock);
                }
            }
        }
		for(unsigned long i = 0; i < clientToRemove.size(); i++)
		{
			for (std::map<int, RRState>::iterator it = _clientSockets.begin(); it != _clientSockets.end();)
			{
				if (it->first == clientToRemove[(int)i])
				{
					std::map<int, RRState>::iterator toErase = it;
					++it;
					_clientSockets.erase(toErase);
				}
				else
				{
				 	it++;
				}
			}
		}
    }
}
