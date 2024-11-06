#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <iostream>

#define BAD_FILE "Invalid file"
#define NO_ACCESS "Cannot acces the file"
#define NO_SERVER_CONFIG "ID points to non-existant server configuration"

class ConfigException : std::exception {
	public:
		virtual const char * what ();
};

class BadPathException : ConfigException {
	public:
		virtual const char * what ();
};

class NoAccessException : ConfigException {
	public:
		virtual const char * what ();
};

class NoServerConfigException : ConfigException {
	public:
		virtual const char * what ();
};


#endif