#include "ErrorHandler.hpp"
#include "ServerBase.hpp"
#include "configuration_file_parsing/config_parser/ConfigParser.hpp"
#include "configuration_file_parsing/server_config/ServerConfig.hpp"

void handle_signal(int sig) {
    if (sig == SIGINT) {
        std::cerr << YELLOW << "\nCaught signal SIGINT (Ctrl+C). Exiting gracefully..." << RESET << std::endl;
		exit(1);
	}
}

int main(int ac, char **argv)
{
	if (ac != 2) {
		std::cerr << RED << "Uncorrect ammount of arguments , try ./webserv [path_of_config_file]" << RESET << std::endl; 
		return (1);
	}
	ConfigParser	*config;
	ServerBase		ServerBase;
	signal(SIGINT, handle_signal);
	try {
		config = ConfigParser::getInstance(argv[1]);
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
