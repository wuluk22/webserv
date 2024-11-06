#include "ServerConfig.hpp"

// Server Configuration class

ServerConfig::ServerConfig(void) {
    this->_server_params_defined = false;
}

ServerConfig::ServerConfig(const ServerConfig &copy) {
    (*this) = copy;
}

ServerConfig::~ServerConfig() {}

ServerConfig& ServerConfig::operator=(const ServerConfig& rhs) {
    if (this != &rhs)
    {
        this->_server_params_defined = rhs._server_params_defined;
        this->_all_directives.assign(rhs._all_directives.begin(), rhs._all_directives.end());
    }
    return (*this);
}

std::vector<Directive> ServerConfig::getAllDirectives(void) const {
    return (this->_all_directives);
}

void ServerConfig::setDirective(Directive new_directive) {
    this->_all_directives.push_back(new_directive);
}

void ServerConfig::addLocationDirective(s_common_params c_params, s_loc_params l_params) {
    LocationBlock new_location_directive(c_params, l_params);
    setDirective(new_location_directive);
}

void ServerConfig::addServerDirective(s_common_params c_params, s_server_params s_params) {
    if (this->_server_params_defined)
        throw ConfigException(TWO_SERVER_BLOCK_DEFINITIONS);
    ServerBlock new_server_directive(c_params, s_params);
    setDirective(new_server_directive);

}

// Directive class and its derivatives
// TODO : Change to abstract, no implementation needed

Directive::Directive(void) {}

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

// LocationBlock

ServerBlock& ServerBlock::operator=(const ServerBlock &rhs) {

}

LocationBlock::LocationBlock(s_common_params common_params, s_loc_params location_params) {

}

LocationBlock::LocationBlock(const LocationBlock &copy) {

}

LocationBlock::~LocationBlock() {}

LocationBlock& LocationBlock::operator=(const LocationBlock &rhs) {

}

