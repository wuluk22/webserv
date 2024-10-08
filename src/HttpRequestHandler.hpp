#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP
# include <sys/socket.h>
# include <cstdlib>
# include <iostream>
# include <cstring>
# include <unistd.h>

class HttpRequestHandler
{
	public:
		static void handle_request(int client_sock);
};

#endif
