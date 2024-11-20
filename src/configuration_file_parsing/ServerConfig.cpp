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
		throw std::exception();
	ServerBlock new_server_directive(c_params, s_params);
	setDirective(new_server_directive);
}

bool ServerConfig::correctAmmountOfServerDirective(void) {
	int server_block_count;
	
	for(std::vector<ADirective>::iterator it = _all_directives.begin(); it != _all_directives.end(); it++) {
		if ((*it).getCommonParams().server_level)
			server_block_count++;
		if (server_block_count > 1)
			return (false);
	}
	return (true);
}

// ADirective class

ADirective::ADirective(void) {}

ADirective::~ADirective() {}

s_common_params ADirective::getCommonParams(void) const {
	return (this->_common_params);
}

bool ADirective::setRoot(std::string root_args) {
	if (root_args.empty())
		return (false);
	_validator.setPath(root_args);
	if (_validator.exists() && _validator.isDirectory() && _validator.isReadable()) {
		this->_common_params._root = root_args;
		return (true);
	}
	return (false);
}

bool ADirective::setIndex(std::set <std::string>index_args) {
	if (index_args.empty())
		return (false);
	for (std::set<std::string>::iterator it = index_args.begin(); it != index_args.end(); ++it)
		_common_params._index.push_back(*it);
	return (true);
}

void ADirective::setAutoIndex(bool value) {
	_common_params._auto_index = value;
}

void ADirective::setClientMaxBodySize(unsigned int body_size_value) {
	_common_params._client_max_body_size = body_size_value;
}

std::string ADirective::getRoot(void) const {
	return (this->_common_params._root);
}

// std::ostream& operator<<(std::ostream& os, const s_common_params& params) {
// 	os << "Root: " << params._root << "\n"
// 		<< "Index: ";
// 	for (const std::string& idx : params._index) {
// 		os << idx << " ";
// 	}
// 	os << "\nAutoIndex: " << (params._auto_index ? "enabled" : "disabled") << "\n"
// 		<< "Client Max Body Size: " << params._client_max_body_size;
// 	return (os);
// }

// ServerBlock

ServerBlock::ServerBlock(void) {
	this->_common_params.server_level = true;
}

ServerBlock::ServerBlock(s_common_params common_params, s_server_params server_params) {
	this->_common_params = common_params;
	this->_server_params = server_params;
}

ServerBlock::ServerBlock(const ServerBlock &copy) {
	(*this) = copy;
}

ServerBlock::~ServerBlock() {}

s_server_params ServerBlock::getServerParams(void) const {
	return (this->_server_params);
}

ServerBlock& ServerBlock::operator=(const ServerBlock &rhs) {
	if (this != &rhs)
	{
		this->_common_params = rhs._common_params;
		this->_server_params = rhs._server_params;
	}
	return (*this);
}

bool ServerBlock::setServerName(std::set<std::string> server_names) {
	if (server_names.empty())
		return (false);
	for (std::set<std::string>::iterator it = server_names.begin(); it != server_names.end(); it++) {
		
	}
	return (true);
}

bool ServerBlock::setListeningPort(std::set<unsigned int> listening_ports) {
	std::set <unsigned int> unique_ports;
	if (listening_ports.empty())
		return (false);
	for (std::set<unsigned int>::iterator it = unique_ports.begin(); it != unique_ports.end(); ++it)
		_server_params._listen.insert(*it);
	return (true);
}

// std::ostream& operator<<(std::ostream& os, const s_server_params& params) {
// 	os << "Server Names: ";
// 	for (const std::string& name : params._server_name) {
// 		os << name << " ";
// 	}
// 	os << "\nListen Ports: ";
// 	for (const unsigned int& port : params._listen) {
// 		os << port << " ";
// 	}
// 	return (os);
// }

// LocationBlock

LocationBlock::LocationBlock(void) {
	this->_common_params.server_level = false;
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
	return (*this);
}

s_loc_params LocationBlock::getLocationParams(void) const {
	return (this->_location_params);
}

bool LocationBlock::setCgiPath(std::string path_args) {
	if (path_args.empty())
		return (false);
	_validator.setPath(path_args);
	if (_validator.exists() && _validator.isFile() && _validator.isExecutable()) {
		_location_params._cgi_path = path_args;
		return (true);
	}
	return (false);
}

bool LocationBlock::setUri(std::string uri_args) {
	if (uri_args.empty())
		return (false);
	_validator.setPath(uri_args);
	if (_validator.exists() && _validator.isDirectory())
	{
		_location_params._uri = uri_args;
		return (true);
	}
	return (false);
}

bool LocationBlock::setAlias(std::string alias_path) {
		if (alias_path.empty())
		return (false);
	_validator.setPath(alias_path);
	if (_validator.exists() && _validator.isDirectory())
	{
		_location_params._alias = alias_path;
		return (true);
	}
	return (false);
}

bool LocationBlock::setAllowedMethods(unsigned char allowed_method) {
	if (allowed_method == 0)
		return (false);
	_location_params._allowed_methods |= allowed_method;
	return (true);
}

bool LocationBlock::isGetAllowed(void) {
	return ((_location_params._allowed_methods & GET) != 0);
}

bool LocationBlock::isPostAllowed(void) {
	return ((_location_params._allowed_methods & POST) != 0);
}

bool LocationBlock::isDeleteAllowed(void) {
	return ((_location_params._allowed_methods & DELETE) != 0);
}

// std::ostream& operator<<(std::ostream& os, const s_loc_params& params) {
// 	os << "Is CGI: " << (params._is_cgi ? "true" : "false") << "\n"
// 		<< "CGI Path: " << params._cgi_path << "\n"
// 		<< "URI: " << params._uri << "\n"
// 		<< "Alias: " << params._alias << "\n"
// 		<< "Allowed Methods: ";
// 	if (params._allowed_methods & GET) os << "GET ";
// 	if (params._allowed_methods & POST) os << "POST ";
// 	if (params._allowed_methods & DELETE) os << "DELETE ";
// 	return (os);
// }
