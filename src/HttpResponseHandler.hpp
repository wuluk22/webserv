#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP
# include <iostream>

class HttpResponseHandler
{
	public:
		void		setStatusCode(int code);
		void		setBody(std::string body);
		int			getStatusCode() const;
		std::string	getBody() const;
	private:
		int			code;
		std::string	body;
};
std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i);

#endif
