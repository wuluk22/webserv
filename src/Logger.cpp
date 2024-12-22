#include "Logger.hpp"

Logger::Logger(void) {}

Logger::Logger(std::string access_log_path, std::string error_log_path)
{
	if (!access_log_path.empty())
	{
		_access_log_output_stream.open(access_log_path.c_str());
		if (!_access_log_output_stream.is_open())
			std::cerr << RED << "Access Log file could not be opened, resuming init";
	}
	if (!error_log_path.empty())
	{
		_error_log_output_stream.open(error_log_path.c_str());
		if (!_error_log_output_stream.is_open())
			std::cerr << RED << "Error Log file could not be opened, resuming init";
	}
}

Logger::~Logger(void)
{
	if (_access_log_output_stream.is_open())
		_access_log_output_stream.close();
	if (_error_log_output_stream.is_open())
		_error_log_output_stream.close();
}

std::string Logger::outputTimestamp(void)
{
	char 		timestamp[20];
	std::time_t	now = std::time(NULL);
	std::tm*	now_tm = std::localtime(&now);
	std::strftime(timestamp, sizeof(timestamp), "%Y/%m/%d - %H:%M", now_tm);
	std::string result(timestamp);
	return (result);
}

void Logger::genericMessage(std::string color, std::string header, std::string msg, std::ostream &output_stream, std::ofstream& output_file_stream)
{
	std::string fully_fledged_message;
	std::string timestamp = outputTimestamp();
	fully_fledged_message = color + header + " [" + timestamp + "] "  + ": " + msg + RESET + '\n';
	output_stream << fully_fledged_message;
	output_stream.flush();
	if (output_file_stream.is_open())
		output_file_stream << fully_fledged_message;
}

void Logger::info(std::string msg)
{
	genericMessage(GREEN, INFO, msg, std::cout, _access_log_output_stream);
}

void Logger::warn(std::string msg)
{
	genericMessage(CYAN, WARN, msg, std::cerr, _error_log_output_stream);
}

void Logger::error(std::string msg)
{
	genericMessage(YELLOW, ERROR,msg, std::cerr, _error_log_output_stream);

}

void Logger::critical(std::string msg)
{
	genericMessage(RED, CRITICAL, msg, std::cerr, _error_log_output_stream);
}
