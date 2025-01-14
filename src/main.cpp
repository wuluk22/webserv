#include "ErrorHandler.hpp"
#include "ServerBase.hpp"
#include "configuration_file_parsing/config_parser/ConfigParser.hpp"
#include "configuration_file_parsing/server_config/ServerConfig.hpp"

volatile sig_atomic_t exit_flag = 0;

void handleSignal(int sig) {
    if (sig == SIGINT) {
        std::cerr << YELLOW << "\nCaught signal SIGINT (Ctrl+C). Exiting gracefully..." << RESET << std::endl;
        exit_flag = 1;
    }
}

int deleteConfig(ConfigParser *config) {
	delete config;
	return (1);
}

int main(int ac, char **argv) {
    if (ac != 2) {
        std::cerr << RED << "Incorrect amount of arguments, try ./webserv [path_of_config_file]" << RESET << std::endl;
        return (1);
    }
    ConfigParser* config = NULL;
    ServerBase ServerBase;
    signal(SIGINT, handleSignal);
    try {
        config = ConfigParser::getInstance(argv[1]);
        ServerConfig* c = config->getServerConfig(0);
        ServerBlock* server_header = c->getServerHeader();
        ServerBase.addPortAndServers(config->getAllServerConfig());
        while (!exit_flag) {
            ServerBase.processClientConnections();
        }
    } 
    catch (ServerHandlerError& e) {
        std::cerr << "ERROR -> " << e.what() << std::endl;
        if (config)
			delete config;
        return (1);
    } 
    catch (ServerBaseError& e) {
        std::cerr << "ERROR -> " << e.what() << std::endl;
        if (config)
			delete config;
        return (1);
    } 
    catch (ConfigParserError& e) {
        std::cerr << "ERROR -> " << e.what() << std::endl;
        if (config) 
			delete config;
        return (1);
    }
    if (config)
        delete config;
    return (0);
}
