#include "LocationBlock.hpp"

LocationBlock::LocationBlock(void) {
	this->_location_params._modified_client_max_body_size = false;
	this->_location_params._allowed_methods = 0;
	this->_location_params._return_args._status_code = NO_RETURN;
}

LocationBlock::~LocationBlock() {}

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
	_validator.setPath(path_args);
	if (_validator.exists() && _validator.isFile() && _validator.isExecutable() && _validator.isReadable()) {
		_location_params._cgi_path = path_args;
		return (true);
	}
	return (false);
}

bool LocationBlock::setUri(std::string uri_args, std::string root_args) {
	if (uri_args.empty() || root_args.empty())
		return (false);
	_validator.setPath(root_args + uri_args);
	if (_validator.exists() && _validator.isDirectory()) {
		_location_params._uri = uri_args;
		return (true);
	}
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
	_validator.setPath(content_path);
	if (_validator.exists() && _validator.isDirectory() && _validator.isReadable()) {
		_location_params._content_path = content_path;
		return (true);
	}
	return (false);
}

std::vector <std::string> LocationBlock::accessibleIndex(void) {
	std::string index_file_path;
	std::vector <std::string> list_of_indices;
	
	for (std::set<std::string>::iterator it = this->_common_params._index.begin(); it != this->_common_params._index.end(); it++) {
		index_file_path = this->_location_params._content_path + "/" + (*it);
		_validator.setPath(index_file_path);
		if (_validator.exists() && _validator.isFile() && _validator.isReadable()) {
			list_of_indices.push_back(this->_location_params._uri + '/' + (*it));
		}
	}
	return (list_of_indices);
}

e_data_reach LocationBlock::isContentPathReachable(void) {
	_validator.setPath(_location_params._content_path);
	if (!_validator.exists())
		return (NO_DATA);
	if (_validator.isReadable() && _validator.isDirectory())
		return (DATA_OK);
	return (DATA_NOK);
}

e_data_reach LocationBlock::isCgiPathReachable(void){
	_validator.setPath(_location_params._cgi_path);
	if (!_validator.exists())
		return (NO_DATA);
	else if (_validator.isReadable() && _validator.isFile() && _validator.isExecutable())
		return (DATA_OK);
	return (DATA_NOK);
}

std::pair<std::string, e_data_reach> LocationBlock::checkAvailableIndex(void) {
	std::string full_file_path;
	std::pair <std::string, e_data_reach> result;
	bool hasEntered = false;
	std::string uri = this->_location_params._uri;

	std::set<std::string>::iterator it = this->_common_params._index.begin();
	for (; it != this->_common_params._index.end(); it++) {
		if (uri[uri.size() - 1] == '/')
			full_file_path = this->_location_params._content_path + (*it);
		else
			full_file_path = this->_location_params._content_path + "/" + (*it);
		_validator.setPath(full_file_path);
		if (_validator.exists() && _validator.isFile()) {
			hasEntered = true;
			if (_validator.isReadable()) {
				result.first = full_file_path;
				result.second = DATA_OK;
			} else {
				result.first = full_file_path;
				result.second = DATA_NOK;
			}
		} else
			continue;
	}
	if (!hasEntered) {
		result.first = full_file_path;
		result.second = NO_DATA;
	}
	return (result);
}

void LocationBlock::setReturnArgs(std::size_t status_code, std::string redirection_url) {
	this->_location_params._return_args._status_code = status_code;
	this->_location_params._return_args._redirection_url = redirection_url;
}

void LocationBlock::setUriDependance(std::string uri) {
	this->_location_params._uri_dependance = uri;
}

// **************************************************************************************

std::string LocationBlock::getUriDependance(void) const {
	return (this->_location_params._uri_dependance);
}

bool LocationBlock::isCgiAllowed(void) const {
	return (!this->_location_params._cgi_path.empty());
}

std::string LocationBlock::getCgiPath(void) const {
	return (this->_location_params._cgi_path);
}

// std::string LocationBlock::getAlias(void) const {
// 	return (this->_location_params._content_path);
// }

std::string LocationBlock::getUri(void) const {
	return (this->_location_params._uri);
}

std::string LocationBlock::getContentPath(void) const {
	return (this->_location_params._content_path);
}

s_return LocationBlock::getReturnArgs(void) const {
	return (this->_location_params._return_args);
}

// bool LocationBlock::isDirectiveCgi(void) const {
// 	return (!this->_location_params._cgi_path.empty());
// }

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
	
	static_cast<const ADirective *>(params)->printAttributes(os);
	if (params->isCgiAllowed()) {
		os	<< "CGI Path: " << params->getCgiPath() << "\n";
	}
	os	<< "URI: " << params->getUri() << "\n"
		<< "Content Path: " << params->getContentPath() << "\n"
		<< "Allowed Methods: " << "\n"
		<< "URI Dependance: " << params->getUriDependance() << "\n";
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
