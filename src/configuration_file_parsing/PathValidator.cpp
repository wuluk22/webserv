#include "PathValidator.hpp"

PathValidator::PathValidator(void) {}

PathValidator::PathValidator(const std::string &path) : _path(path) {}

void PathValidator::setPath(const std::string &path) {
	this->_path = path;
}

bool PathValidator::exists() const {
	struct stat info;
	return (stat(_path.c_str(), &info) == 0);
}

bool PathValidator::isFile() const {
	struct stat info;
	if (stat(_path.c_str(), &info) != 0)
		return false;
	return (S_ISREG(info.st_mode));
}

bool PathValidator::isDirectory() const {
	struct stat info;
	if (stat(_path.c_str(), &info) != 0) 
		return false;
	return (S_ISDIR(info.st_mode));
}

bool PathValidator::isReadable() const {
	return (access(_path.c_str(), R_OK) == 0);
}

bool PathValidator::isWritable() const {
	return (access(_path.c_str(), W_OK) == 0);
}
