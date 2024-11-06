#include "ConfigException.hpp"

const char * ConfigException::what() {
	return ("Default Config Exception");
}

const char * BadPathException::what() {
	return (BAD_FILE);
}

const char * NoAccessException::what() {
	return (NO_ACCESS);
}

const char * NoServerConfigException::what() {
	return (NO_SERVER_CONFIG);
}