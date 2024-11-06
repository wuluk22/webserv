#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <iostream>

class ConfigException : public std::exception {
	private:
		std::string message;
	public:
		ConfigException(const std::string& msg);
		virtual const char* what() const throw();
};

#endif