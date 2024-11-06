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

std::vector<ADirective> ServerConfig::getAllDirectives(void) const {
	return (this->_all_directives);
}

void ServerConfig::setDirective(ADirective new_directive) {
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

ADirective::ADirective(void) {}

// ServerBlock

ServerBlock::ServerBlock(s_common_params common_params, s_server_params server_params) {
	this->_common_params = common_params;
	this->_server_params = server_params;
}

ServerBlock::ServerBlock(const ServerBlock &copy) {
	(*this) = copy;
}

ServerBlock::~ServerBlock() {}

s_common_params ServerBlock::getCommonParams(void) const {
	return (this->_common_params);
}

s_server_params ServerBlock::getServerParams(void) const {
	return (this->_server_params);
}

void ServerBlock::setCommonParams(s_common_params common_params) {
	this->_common_params = common_params;
}

void ServerBlock::setServerParams(s_server_params server_params) {
	this->_server_params = server_params;
}

// LocationBlock

ServerBlock& ServerBlock::operator=(const ServerBlock &rhs) {
	if (this != &rhs)
	{
		this->_common_params = rhs._common_params;
		this->_server_params = rhs._server_params;
	}
	return (*this);
}

LocationBlock::LocationBlock(s_common_params common_params, s_loc_params location_params) {
	this->_common_params = common_params;
	this->_location_params = location_params;
}

LocationBlock::LocationBlock(const LocationBlock &copy) {
	(*this) = copy;
}

LocationBlock::~LocationBlock() {}

LocationBlock& LocationBlock::operator=(const LocationBlock &rhs) {
	if (this != &rhs)
	{
		this->_common_params = rhs._common_params;
		this->_location_params = rhs._location_params;
	}
}

s_common_params LocationBlock::getCommonParams(void) const {
	return (this->_common_params);
}

s_loc_params LocationBlock::getLocationParams(void) const {
	return (this->_location_params);
}

void LocationBlock::setCommonParams(s_common_params common_params) {
	this->_common_params = common_params;
}

void LocationBlock::setLocationParams(s_loc_params location_params) {
	this->_location_params = location_params;
}
