#ifndef LOGGER_HPP
# define LOGGER_HPP
# include <iostream>

class Logger
{
	public:
		static void log(const std::string message);
		static void log_error(const std::string error_message);
};

#endif
