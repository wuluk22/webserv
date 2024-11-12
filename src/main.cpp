#include "ServerBase.hpp"

// const int BACKLOG = 3;

int main()
{
	ServerBase	ServerBase;

	ServerBase.addPortAndServers();
	for (unsigned long i = 0; i < ServerBase.get_servers().size(); i++) {
		ServerBase.processClientConnections();
	}
    return 0;
}
