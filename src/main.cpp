#include "ErrorHandler.hpp"
#include "ServerBase.hpp"
#include "configuration_file_parsing/ConfigParser.hpp"
#include "configuration_file_parsing/ServerConfig.hpp"

// const int BACKLOG = 3;

int main()
{
	ConfigParser	*config;
	ServerBase		ServerBase;
	try
	{
		config = ConfigParser::getInstance("test.conf");
		ServerConfig* c = config->getServerConfig(0);
		ServerBlock	*server_header = c->getServerHeader();
		ServerBase.addPortAndServers(config->getAllServerConfig());
		// for (unsigned long i = 0; i < ServerBase.getServers().size(); i++)
		// {
			// std::cout << "nbr of servers : " << ServerBase.getServers().size() << std::endl;
			ServerBase.processClientConnections();
		// }
	}
	catch (const ServerHandlerError& e)
	{
		std::cerr << "ERROR -> " << e.what() << std::endl;
	}
	catch (const ServerBaseError& e)
	{
		std::cerr << "ERROR -> " << e.what() << std::endl;
	}
	catch (ConfigParserError &e)
	{
		std::cout << "ERROR -> " << e.what() << std::endl;
	}
    return 0;
}
