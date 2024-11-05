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
		std::string	getMethod(void) const;
		std::string	getPath(void) const;
		std::string	getHttpVersion(void) const;

		void		setMethod(const std::string &m);
		void		setPath(const std::string &p);
		void		setHttpVersion(const std::string &h);

		static void handle_request(int client_sock);
		HttpRequestHandler	httpParsing(std::string buffer);
		//HttpRequestHandler&	operator=(const HttpRequestHandler &assign);
	private:
		std::map<std::string, std::string> headers;
		std::string	method;
		std::string	path;
		std::string	httpVersion;
};

#endif
