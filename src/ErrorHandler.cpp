#include "ErrorHandler.hpp"
#include "HttpRequestHandler.hpp"

std::string toStr(const char* value)
{
    std::ostringstream	oss;

    oss << value;
    return oss.str();
}

std::string toStrInt(int value)
{
    std::ostringstream	oss;

    oss << value;
    return oss.str();
}

// METHODS ERRORS
Error::Error(const std::string& msg) : msg(msg) {};
const char* Error::what() const throw() { return msg.c_str(); }
Error::~Error() throw() {}

ServerHandlerError::ServerHandlerError(const std::string& msg, const char* function, int line) : Error("Error in ServerHandler class in function : " + toStr(function) + " in line : " + toStrInt(line) + " : " + msg) {};
ServerHandlerError::~ServerHandlerError() throw() {}

ServerBaseError::ServerBaseError(const std::string& msg, const char* function, int line) : Error("Error in ServerBase class in function : " + toStr(function) + " in line : " + toStrInt(line) + " : " + msg) {};
ServerBaseError::~ServerBaseError() throw() {} // need to redefine, if not, it's COEXEPT from c++11 because i modified std::exception