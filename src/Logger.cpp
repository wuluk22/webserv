#include "Logger.hpp"

void Logger::log(const std::string message)
{
	std::cout << "LOG: " << message << std::endl;
}

void Logger::logError(const std::string error_message)
{
	std::cerr << "ERROR: " << errorMessage << std::endl;
}
