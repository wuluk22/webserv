#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <iostream>

// ERROR MACRO
#define ERROR_HEADER "\e[91m[CONFIG_PARSER] : "

// ERROR MESSAGES
#define BAD_ACCESS "No such file present, aborting"
#define NO_SERVER_CONFIG "No such server configuration, aborting"
#define TOKEN_REPEATED "Non-repeatable token repeated within block, aborting"
#define INVALID_TOKEN "Invalid token in the configuration file, aborting"
#define NO_INSTRUCTION "No instruction given in the configuration file, aborting"
#define TOKEN_POSITION_MISMATCH "Token is not within any block, aborting"
#define BAD_INSTRUCTION "Bad instruction within a directive, aborting"
#define NO_URI_LOCATION "Location token contain no URI, aborting"
#define BAD_URI "Bad URI, aborting"
#define DUPE_ELEMS "Duplicate elements discovered, aborting"
#define NO_ELEMENTS "Lack of proper arguments, aborting"
#define DOUBLE_DIRECTIVE "Double server directive, aborting"
#define EXCEEDING_LIMIT "Size limit exceeded, aborting"
#define PATH_NOT_RECOGNIZED "Path isn't recognized, aborting"
#define ROOT_PRIORITY "Root redefinition should be declared before inbricked location, aborting"
#define NO_ROOT_DEFINITION "No root definition, vacant URI prefix, aborting"
#define UNDEFINED_PARAMS "Undefined parameter"
#define NUMERICAL_VALUE_EXPECTED "Numerical value expected, aborting"
#define RESERVED_PORTS_LINUX "Ports under 1024 are reserved, aborting"
#define AMBIGUOUS_URI_DEFINITION "Root and alias cannot be declared inside the same location, aborting"
#define WRONG_ERROR_PAGES_SCOPE "Wrong error pages scope, aborting"
#define AL " at line : "
#define RESET "\e[0m"

// EXCEPTIONS MESSAGES
#define DEFAULT_CONFIG_EXCEPTION "Config exception, aborting"

class ConfigException : std::exception {
	public:
		virtual const char * what ();
};

#endif