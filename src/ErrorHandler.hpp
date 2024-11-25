#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <string>
#include <exception>
#include <sstream>

class Error : public std::exception {
	protected:
		std::string msg;
	public:
		explicit Error(const std::string& msg);
		virtual const char* what() const throw();
		virtual ~Error() throw();
};

class ServerHandlerError : public Error {
	public:
		explicit ServerHandlerError(const std::string& msg, const char* function, int line);
		virtual ~ServerHandlerError() throw();
};

class ServerBaseError : public Error {
	public:
		explicit ServerBaseError(const std::string& msg, const char* function, int line);
		virtual ~ServerBaseError() throw();
};

std::string toStrInt(int value);
std::string toStr(const char* value);

#endif
