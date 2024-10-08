#include "Logger.hpp"
#include <iostream>

void Logger::log(const std::string message)
{
	std::cout << "LOG: " << message << std::endl;
}

void Logger::log_error(const std::string error_message)
{
	std::cerr << "ERROR: " << error_message << std::endl;
}
