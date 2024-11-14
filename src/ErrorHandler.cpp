#include "ErrorHandler.hpp"

// METHODS ERRORS
Error::Error(const std::string& msg) : msg(msg) {};
const char* Error::what() const throw() { return msg.c_str(); }
Error::~Error() throw() {}

ServerHandlerError::ServerHandlerError(const std::string& msg, const char* function, int line) : Error("Error in ServerHandler class in function : " + std::string(function) + " in line : " + std::to_string(line) + " : " + msg) {};
ServerHandlerError::~ServerHandlerError() throw() {}

ServerBaseError::ServerBaseError(const std::string& msg, const char* function, int line) : Error("Error in ServerBase class in function : " + std::string(function) + " in line : " + std::to_string(line) + " : " + msg) {};
ServerBaseError::~ServerBaseError() throw() {} // need to redefine, if not, it's COEXEPT from c++11 because i modified std::exception