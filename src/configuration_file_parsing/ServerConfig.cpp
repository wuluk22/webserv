#include "ServerConfig.hpp"

// Server Configuration class

ServerConfig::ServerConfig(void) {
	this->_server_params_defined = false;
}

ServerConfig::ServerConfig(const ServerConfig &copy) {
	(*this) = copy;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
	if (this != &other) {
		this->all_directives = other.all_directives;
		this->_server_params_defined = other._server_params_defined;
	}
	return (*this);
}

ServerConfig::~ServerConfig() {
	for (int i = 0; i < all_directives.size(); i++) {
		delete all_directives[i];
	}
}

std::vector<ADirective *> ServerConfig::getAllDirectives(void) const {
	return (this->all_directives);
}

void ServerConfig::setDirective(ADirective *new_directive) {
	this->all_directives.push_back(new_directive);
}

bool ServerConfig::correctAmmountOfServerDirective(void) {
	int server_block_count;
	
	for(std::vector<ADirective *>::iterator it = all_directives.begin(); it != all_directives.end(); it++) {
		if ((*it)->getCommonParams().server_level)
			server_block_count++;
		if (server_block_count > 1)
			return (false);
	}
	return (true);
}

// ADirective class

ADirective::ADirective(void) {
	this->_common_params._auto_index = false;
	this->_common_params._client_max_body_size = 1;
}

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
		_common_params._index.insert(*it);
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

std::set<std::string> ADirective::getIndex(void) const {
	return (this->_common_params._index);
}

bool ADirective::getAutoIndex(void) const {
	return (this->_common_params._auto_index);
}

unsigned int ADirective::getClientMaxBodySize(void) const {
	return (this->_common_params._client_max_body_size);
}

std::ostream& operator<<(std::ostream& os, const ADirective *params) {
	const std::set<std::string> indexSet = params->getIndex();

	os << "Root : " << params->getRoot() << "\n"
	<< "Auto index : " << params->getAutoIndex() << "\n";
	for (std::set<std::string>::const_iterator it = indexSet.begin(); it != indexSet.end(); ++it) {
		os << "Index : " << (*it) << "\n";
	}
	os	<< "Client Max Body Size : " << params->getClientMaxBodySize() << "\n";
	return (os);
}

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
	this->_server_params._server_name = server_names;
	return (true);
}

bool ServerBlock::setListeningPort(std::set<unsigned int> listening_ports) {
	if (listening_ports.empty())
		return (false);
	this->_server_params._listen = listening_ports;
	return (true);
}

std::set <std::string> ServerBlock::getServerName(void) const {
	return (this->_server_params._server_name);
}

std::set <unsigned int> ServerBlock::getListeningPort(void) const {
	return (this->_server_params._listen);
}

std::ostream& operator<<(std::ostream& os, const ServerBlock *params) {
	const std::set<unsigned int> listening_ports = params->getListeningPort();
	const std::set<std::string> server_names = params->getServerName();

	std::cout << "\n\n" << "SERVER BLOCK" << "\n\n";
	os << static_cast<const ADirective *>(params);
	for (std::set<unsigned int>::iterator it = listening_ports.begin(); it != listening_ports.end(); it++)
		os << "Listening ports : " << (*it) << "\n"; 
	for (std::set<std::string>::iterator it = server_names.begin(); it != server_names.end(); it++)
		os << "Server name : " << (*it) << "\n";
	return (os);
}

// LocationBlock

LocationBlock::LocationBlock(void) {
	this->_common_params.server_level = false;
	this->_location_params._is_cgi = false;
	this->_common_params._client_max_body_size = 1;
	this->_location_params.modified_auto_index = false;
	this->_location_params.modified_client_max_body_size = false;
	this->_location_params._allowed_methods = 0;
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

void LocationBlock::clientMaxBodySizeModified(void) {
	this->_location_params.modified_client_max_body_size = true;
}

void LocationBlock::autoIndexModified(void) {
	this->_location_params.modified_auto_index = true;
}

void LocationBlock::setIsCgi(bool value) {
	this->_location_params._is_cgi = value;
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

bool LocationBlock::isCgiAllowed(void) const {
	return (this->_location_params._is_cgi);
}

std::string LocationBlock::getCgiPath(void) const {
	return (this->_location_params._cgi_path);
}

std::string LocationBlock::getAlias(void) const {
	return (this->_location_params._alias);
}

std::string LocationBlock::getUri(void) const {
	return (this->_location_params._uri);
}

bool LocationBlock::isDirectiveCgi(void) const {
	return (this->_location_params._is_cgi);
}

bool LocationBlock::isGetAllowed(void) const {
	return ((_location_params._allowed_methods & GET) != 0);
}

bool LocationBlock::isPostAllowed(void) const {
	return ((_location_params._allowed_methods & POST) != 0);
}

bool LocationBlock::isDeleteAllowed(void) const {
	return ((_location_params._allowed_methods & DELETE) != 0);
}

std::ostream& operator<<(std::ostream& os, const LocationBlock *params) {
	std::cout << "\n\n" << "LOCATION BLOCK" << "\n\n";
	
	os << static_cast<const ADirective *>(params);
	if (params->isCgiAllowed()) {
		os	<< "CGI Path: " << params->getCgiPath() << "\n";
	}
	os	<< "URI: " << params->getUri() << "\n"
		<< "Alias: " << params->getAlias() << "\n"
		<< "Allowed Methods: ";
	if (params->isGetAllowed()) 
		os << "GET ";
	if (params->isPostAllowed()) 
		os << "POST ";
	if (params->isDeleteAllowed()) 
		os << "DELETE ";
	return (os);
}

bool LocationBlock::hasClientMaxBodySizeModified(void) const {
	return (this->_location_params.modified_client_max_body_size);
}

bool LocationBlock::hasAutoIndexModified(void) const {
	return (this->_location_params.modified_auto_index);
}
