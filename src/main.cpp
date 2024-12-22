#include "ErrorHandler.hpp"
#include "ServerBase.hpp"
#include "configuration_file_parsing/config_parser/ConfigParser.hpp"
#include "configuration_file_parsing/server_config/ServerConfig.hpp"

void handle_signal(int sig) {
    if (sig == SIGINT) {
        std::cerr << "\n Caught signal SIGINT (Ctrl+C). Exiting gracefully..." << std::endl;
        exit(1);
	}
}

int main()
{
	ConfigParser	*config;
	ServerBase		ServerBase;
	signal(SIGINT, handle_signal);
	try {
		config = ConfigParser::getInstance("test.conf");
		ServerConfig* c = config->getServerConfig(0);
		ServerBlock* server_header = c->getServerHeader();
		ServerBase.addPortAndServers(config->getAllServerConfig());
		ServerBase.processClientConnections();
	}
	catch (const ServerHandlerError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
		return (1);
	}
	catch (const ServerBaseError& e) {
		std::cerr << "ERROR -> " << e.what() << std::endl;
		return (1);
	}
	catch (ConfigParserError &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
	delete config;
	return (0);
}
