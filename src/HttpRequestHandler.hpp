#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include <sys/socket.h> // recv | send
# include <iostream>
# include <unistd.h> // close
# include <map>
# include <sstream>

class HttpRequestHandler
{
	public:
		static void handle_request(int client_sock);
		std::string	httpParsing(std::string buffer) const;
	private:
		std::map<std::string, std::string> headers;
		std::string	method;
		std::string	path;
		std::string	httpVersion;
};

#endif
