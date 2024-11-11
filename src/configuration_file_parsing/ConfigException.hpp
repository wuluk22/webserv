#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <iostream>

// ERROR MACRO
#define ERROR_HEADER "[CONFIG_PARSER] : "

// ERROR MESSAGES
#define BAD_ACCESS "No such file present, aborting"
#define NO_SERVER_CONFIG "No such server configuration, aborting"
#define TOKEN_REPEATED "Non-repeatable token repeated within block, aborting"
#define INVALID_TOKEN "Invalid token in the configuration file, aborting"
#define NO_INSTRUCTION "No instruction given in the configuration file, aborting"
#define TOKEN_POSITION_MISMATCH "Token is not within any block, aborting"
#define AL " at line : "

// EXCEPTIONS MESSAGES
#define DEFAULT_CONFIG_EXCEPTION "Config exception, aborting"

class ConfigException : std::exception {
	public:
		virtual const char * what ();
};

#endif