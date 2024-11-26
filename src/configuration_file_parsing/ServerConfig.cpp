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

bool ADirective::setIndex(std::vector<std::string>index_args) {
	std::set <std::string> unique_index;
	if (index_args.empty())
		return (false);
	unique_index.insert(index_args.begin(), index_args.end());
	for (std::set<std::string>::iterator it = unique_index.begin(); it != unique_index.end(); ++it)
		_common_params._index.push_back(*it);
	return (true);
}

void ADirective::setAutoIndex(bool value) {
	_common_params._auto_index = value;
}

void ADirective::setClientMaxBodySize(unsigned int body_size_value) {
	_common_params._client_max_body_size = body_size_value;
}

// ServerBlock

ServerBlock::ServerBlock(void) {}

ServerBlock::ServerBlock(s_common_params common_params, s_server_params server_params) {
	this->_common_params = common_params;
	this->_server_params = server_params;
}

ServerBlock::ServerBlock(const ServerBlock &copy) {
	(*this) = copy;
}

ServerBlock::~ServerBlock() {}

bool ServerBlock::isValidServerName(std::string name) {
	bool last_dot_occurence;

	last_dot_occurence = false;
	if (name.empty())
		return (false);
	if (!std::isalpha(name[0]))
		return (false);
	for (int i = 0; i < name.size(); i++) {
		char c = name[i];
		if (std::isalnum(c) || c == '-' || c == '.') {
			if ((i == 0 || i == name.size() - 1) && (c == '.' || c == '-')) 
				return false;
			if (c == '.') {
				if (last_dot_occurence) 
					return (false);
				last_dot_occurence = true;
			} else
				last_dot_occurence = false;
		} else
			return (false);
	}
	return (true);
}

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

bool ServerBlock::setServerName(std::vector<std::string> server_names) {
	if (server_names.empty())
		return (false);
	for (int i = 0; i < server_names.size(); i++) {
		if (isValidServerName(server_names[i])) {
			_server_params._server_name.push_back(server_names[i]);
		} else
			return (false);
	}
	return (true);
}

bool ServerBlock::setListeningPort(std::vector<unsigned int> listening_ports) {
	std::set <unsigned int> unique_ports;
	if (listening_ports.empty())
		return (false);
	for (std::set<unsigned int>::iterator it = unique_ports.begin(); it != unique_ports.end(); ++it)
    	_server_params._listen.push_back(*it);
	return (true);
}

// LocationBlock

LocationBlock::LocationBlock(void) {}

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
	if (allowed_method == 0) {
		return (false);
	}
	_location_params._allowed_methods = allowed_method;
	return (true);
}

bool LocationBlock::isGetAllowed(void) {
	return (_location_params._allowed_methods & GET) != 0;
}

bool LocationBlock::isPostAllowed(void) {
	return (_location_params._allowed_methods & POST) != 0;
}

bool LocationBlock::isDeleteAllowed(void) {
	return (_location_params._allowed_methods & DELETE) != 0;
}
