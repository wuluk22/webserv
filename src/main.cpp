#include "ErrorHandler.hpp"
#include "ServerBase.hpp"

// const int BACKLOG = 3;

int main()
{
	ServerBase	ServerBase;
	try {
		ServerBase.addPortAndServers();
		for (unsigned long i = 0; i < ServerBase.get_servers().size(); i++) {
			std::cout << "nbr of servers : " << ServerBase.get_servers().size() << std::endl;
			ServerBase.processClientConnections();
		}
	} catch (const ServerHandlerError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
	} catch (const ServerBaseError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
	}
    return 0;
}
