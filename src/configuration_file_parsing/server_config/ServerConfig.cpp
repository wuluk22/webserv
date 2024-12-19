#include "ServerConfig.hpp"

ServerConfig::ServerConfig(void) {
	this->server_header = NULL;
}

ServerConfig::~ServerConfig() {
	for (int i = 0; i < directives.size(); i++) {
		delete directives[i];
	}
	if (server_header)
		delete server_header;
}

std::vector<LocationBlock *> ServerConfig::getDirectives(void) const {
	return (this->directives);
}

void ServerConfig::setDirective(LocationBlock *new_directive) {
	this->directives.push_back(new_directive);
}

void ServerConfig::setServerHeader(ServerBlock *server_header) {
	this->server_header = server_header;
}

ServerBlock* ServerConfig::getServerHeader(void) const {
	return (this->server_header);
}
