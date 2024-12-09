#include "ServerConfig.hpp"

// Server Configuration class

ServerConfig::ServerConfig(void) {
	this->server_header = NULL;
}

ServerConfig::ServerConfig(const ServerConfig &copy) {
	(*this) = copy;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
	if (this != &other) {
		this->directives = other.directives;
		this->server_header = other.server_header;
	}
	return (*this);
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
	if (IS_LINUX)
		this->_server_params._listen.insert(1024);
	else
		this->_server_params._listen.insert(80);
	listening_ports_set = false;
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
	if (this != &rhs) {
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
	if (!listening_ports_set) {
		this->_server_params._listen = listening_ports;
		listening_ports_set = true;
	} else {
		for (std::set<unsigned int>::iterator it = listening_ports.begin(); it != listening_ports.end(); it++) {
			this->_server_params._listen.insert(*it);
		}
	}
	return (true);
}

bool ServerBlock::setErrorPagesRecord(std::map<unsigned int, std::string> error_pages_record) {
	unsigned int error_value;
	std::pair<unsigned int, std::string> current_pair;
	
	if (error_pages_record.empty())
		return (false);
	for (std::map<unsigned int, std::string>::iterator it = error_pages_record.begin(); it != error_pages_record.end(); it++) {
		current_pair = (*it);
		error_value = current_pair.first;
		this->_server_params._error_pages_record.erase(error_value);
		this->_server_params._error_pages_record.insert(*it);
	}
	return (true);
}

std::set <std::string> ServerBlock::getServerName(void) const {
	return (this->_server_params._server_name);
}

std::set <unsigned int> ServerBlock::getListeningPort(void) const {
	return (this->_server_params._listen);
}

std::map<unsigned int, std::string> ServerBlock::getErrorPagesRecord(void) const {
	return (this->_server_params._error_pages_record);
}

std::ostream& operator<<(std::ostream& os, const ServerBlock *params) {
	const std::set<unsigned int> listening_ports = params->getListeningPort();
	const std::set<std::string> server_names = params->getServerName();
	const std::map<unsigned int, std::string> error_pages_record = params->getErrorPagesRecord();

	std::cout << "\n\n" << "SERVER BLOCK" << "\n\n";
	os << static_cast<const ADirective *>(params);
	for (std::set<unsigned int>::iterator it = listening_ports.begin(); it != listening_ports.end(); it++)
		os << "Listening ports : " << (*it) << "\n"; 
	for (std::set<std::string>::iterator it = server_names.begin(); it != server_names.end(); it++)
		os << "Server name : " << (*it) << "\n";
	for (std::map<unsigned int, std::string>::const_iterator it = error_pages_record.begin(); it != error_pages_record.end(); it++) {
		if (it == error_pages_record.begin())
			os << "Error pages redefinition" << "\n";
		os << "Port : " << (*it).first << " | Path : " << (*it).second << std::endl;
	}
	return (os);
}

// LocationBlock

LocationBlock::LocationBlock(void) {
	this->_common_params._client_max_body_size = 1;
	this->_location_params._modified_auto_index = false;
	this->_location_params._modified_client_max_body_size = false;
	this->_location_params._allowed_methods = 0;
	this->_location_params._return_args._status_code = NO_RETURN;
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
	this->_location_params._modified_client_max_body_size = true;
}

void LocationBlock::autoIndexModified(void) {
	this->_location_params._modified_auto_index = true;
}

bool LocationBlock::setCgiPath(std::string path_args) {
	if (path_args.empty()) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	_validator.setPath(path_args);
	if (_validator.exists() && _validator.isFile() && _validator.isExecutable() && _validator.isReadable()) {
		_location_params._cgi_path = path_args;
		return (true);
	}
	std::cerr << ERROR_HEADER << BAD_ACCESS << RESET << std::endl;
	return (false);
}

bool LocationBlock::setUri(std::string uri_args, std::string root_args) {
	if (uri_args.empty() || root_args.empty()) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	_validator.setPath(root_args + uri_args);
	if (_validator.exists() && _validator.isDirectory()) {
		_location_params._uri = uri_args;
		return (true);
	}
	std::cerr << ERROR_HEADER << BAD_ACCESS << RESET << std::endl;
	return (false);
}

bool LocationBlock::setAlias(std::string alias_path) {
	return (setContentPath(alias_path));
}

bool LocationBlock::setAllowedMethods(unsigned char allowed_method) {
	if (allowed_method == 0)
		return (false);
	_location_params._allowed_methods |= allowed_method;
	return (true);
}

bool LocationBlock::setContentPath(std::string content_path) {
	if (content_path.empty()) {
		std::cerr << ERROR_HEADER << NO_ELEMENTS << RESET << std::endl;
		return (false);
	}
	_validator.setPath(content_path);
	if (_validator.exists() && _validator.isDirectory() && _validator.isReadable()) {
		_location_params._content_path = content_path;
		return (true);
	}
	std::cerr << ERROR_HEADER << BAD_ACCESS << RESET << std::endl;
	return (false);
}

std::string LocationBlock::getFirstAccessibleIndex(void) {
	std::string index_file_path;
	
	for (std::set<std::string>::iterator it = this->_common_params._index.begin(); it != this->_common_params._index.end(); it++) {
		index_file_path = this->_location_params._content_path + "/" + (*it);
		_validator.setPath(index_file_path);
		if (_validator.exists() && _validator.isFile() && _validator.isReadable())
			return (index_file_path);
	}
	return ("");
}

bool LocationBlock::setReturnArgs(std::size_t status_code, std::string redirection_url) {
	this->_location_params._return_args._status_code = status_code;
	this->_location_params._return_args._redirection_url = redirection_url;
	return (true);
}

bool LocationBlock::isCgiAllowed(void) const {
	return (!this->_location_params._cgi_path.empty());
}

std::string LocationBlock::getCgiPath(void) const {
	return (this->_location_params._cgi_path);
}

std::string LocationBlock::getAlias(void) const {
	return (this->_location_params._content_path);
}

std::string LocationBlock::getUri(void) const {
	return (this->_location_params._uri);
}

std::string LocationBlock::getContentPath(void) const {
	return (this->_location_params._content_path);
}

s_return LocationBlock::getReturnArgs(void) const {
	return (this->_location_params._return_args);
}

bool LocationBlock::isDirectiveCgi(void) const {
	return (!this->_location_params._cgi_path.empty());
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

bool LocationBlock::hasClientMaxBodySizeModified(void) const {
	return (this->_location_params._modified_client_max_body_size);
}

bool LocationBlock::hasAutoIndexModified(void) const {
	return (this->_location_params._modified_auto_index);
}

std::ostream& operator<<(std::ostream& os, const LocationBlock *params) {
	std::cout << "\n\n" << "LOCATION BLOCK" << "\n\n";
	
	os << static_cast<const ADirective *>(params);
	if (params->isCgiAllowed()) {
		os	<< "CGI Path: " << params->getCgiPath() << "\n";
	}
	os	<< "URI: " << params->getUri() << "\n"
		<< "Content Path: " << params->getContentPath() << "\n"
		<< "Allowed Methods: " << "\n";
	if (params->getReturnArgs()._status_code != NO_RETURN)
		os << "Return status code : " << params->getReturnArgs()._status_code << "\n";
	if (!params->getReturnArgs()._redirection_url.empty())
		os << "Redirection URL : " << params->getReturnArgs()._redirection_url << "\n";
	if (params->isGetAllowed()) 
		os << "GET ";
	if (params->isPostAllowed()) 
		os << "POST ";
	if (params->isDeleteAllowed()) 
		os << "DELETE ";
	return (os);
}
