#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include <sys/socket.h>
# include <unistd.h>
# include <iostream>

class HttpRequestHandler
{
	public:
		static int handle_request(int client_sock);
};

#endif
