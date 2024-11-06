#include "ServerConfig.hpp"
#include <vector>

// Server Configuration class

ServerConfig::ServerConfig(void) {}

ServerConfig::ServerConfig(const ServerConfig &copy) {

}

ServerConfig::~ServerConfig() {}

ServerConfig& ServerConfig::operator=(const ServerConfig& rhs) {

}

std::vector<Directive> ServerConfig::getAllDirectives(void) const {

}

void ServerConfig::setDirective(Directive new_directive) {

}

void ServerConfig::addLocationDirective(s_common_params c_params, s_loc_params l_params) {

}

void ServerConfig::addServerDirective(s_common_params c_params, s_server_params s_params) {
    
}

// Directive class and its derivatives

Directive::Directive(void) {

}

Directive::Directive(const ServerConfig &copy) {

}

Directive::~Directive() {}

Directive& Directive::operator=(const Directive &rhs) {

}

// ServerBlock

ServerBlock::ServerBlock(s_common_params common_params, s_server_params server_params) {

}

ServerBlock::ServerBlock(const ServerBlock &copy) {

}

ServerBlock::~ServerBlock() {

}

ServerBlock& ServerBlock::operator=(const ServerBlock &rhs) {

}

LocationBlock::LocationBlock(s_common_params common_params, s_loc_params location_params) {

}

LocationBlock::LocationBlock(const LocationBlock &copy) {

}

LocationBlock::~LocationBlock() {}

LocationBlock& LocationBlock::operator=(const LocationBlock &rhs) {

}