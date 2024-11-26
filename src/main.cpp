#include "ErrorHandler.hpp"
#include "ServerBase.hpp"
#include "configuration_file_parsing/ConfigParser.hpp"
#include "configuration_file_parsing/ServerConfig.hpp"

// const int BACKLOG = 3;

int main()
{
	ConfigParser *config;
	ServerBase	ServerBase;
	try {
		config = ConfigParser::getInstance("test.conf");
		ServerConfig* c = config->getServerConfig(0);
		std::vector <ADirective *> all_directives = c->getAllDirectives();
		
		ServerBase.addPortAndServers(c);
		for (unsigned long i = 0; i < ServerBase.get_servers().size(); i++) {
			std::cout << "nbr of servers : " << ServerBase.get_servers().size() << std::endl;
			ServerBase.processClientConnections();
		}
	} catch (const ServerHandlerError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
	} catch (const ServerBaseError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
	} catch (ConfigException &e) {
		std::cout << "ERROR -> " << e.what() <<std::endl;
	}
    return 0;
}
