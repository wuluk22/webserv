#ifndef HTTPRESPONSEHANLDER_HPP
#define HTTPRESPONSEHANLDER_HPP

# include <sys/socket.h>
# include <iostream>

class HttpResponseHandler
{
	private:
	public:
		static int	handle_response(int client_sock);
};

#endif
