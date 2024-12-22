#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <ctime>
# include <fstream>
# include <string>
# include <iostream>

// COLORS MACRO
#ifndef RESET
	#define RESET	"\x1b[0m"
#endif
#define RED		"\x1b[31m"
#define GREEN	"\x1b[32m"
#define YELLOW	"\x1b[33m"
#define CYAN	"\x1b[36m"

// LOG LEVEL MACROS
#define INFO		"[INFO]"
#define WARN		"[WARN]"
#define ERROR		"[ERROR]"
#define CRITICAL 	"[CRIT]" 

class Logger
{
	private:
		std::string		_error_log_path;
		std::string		_access_log_path;
		std::ofstream	_error_log_output_stream;
		std::ofstream	_access_log_output_stream;
		std::string		outputTimestamp(void) ;
		void			genericMessage(std::string color, std::string header, std::string msg, std::ostream &output_stream, std::ofstream& output_file_stream);
	public:
		Logger(void);
		~Logger(void);
		Logger(std::string access_log_path, std::string error_log_path);
		void info(std::string msg);
		void warn(std::string msg);
		void error(std::string msg);
		void critical(std::string msg);
};

#endif
