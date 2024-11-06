#include "ConfigException.hpp"

ConfigException::ConfigException(const std::string& msg) : message(msg) {}

const char* ConfigException::what() const throw() {
	return (message.c_str());
}