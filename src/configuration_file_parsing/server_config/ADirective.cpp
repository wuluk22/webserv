#include "ADirective.hpp"

ADirective::ADirective(void) {
	this->_common_params._index.insert("index.htm");
	this->_common_params._index.insert("index.html");
	this->_common_params._is_index_files_init = false;
	this->_common_params._auto_index = false;
	this->_common_params._client_max_body_size = 1;
}

ADirective::~ADirective() {}

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

void ADirective::setIndex(std::set <std::string>index_args) {
	if (!_common_params._is_index_files_init) {
		_common_params._index.clear();
		_common_params._is_index_files_init = true;
	}
	for (std::set<std::string>::iterator it = index_args.begin(); it != index_args.end(); ++it)
		_common_params._index.insert(*it);
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