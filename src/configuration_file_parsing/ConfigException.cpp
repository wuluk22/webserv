#include "ConfigException.hpp"

const char * ConfigException::what() {
	return (DEFAULT_CONFIG_EXCEPTION);
}
