#include "HttpResponseHandler.hpp"

std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i)
{
	out << i.getStatusCode() << std::endl;
	out << i.getBody() << std::endl;
	return out;
}

void	HttpResponseHandler::setStatusCode(int code)
{
	this->code = code;
}

void	HttpResponseHandler::setBody(std::string body)
{
	this->body = body;
}

int		HttpResponseHandler::getStatusCode() const
{
	return code;
}

std::string	HttpResponseHandler::getBody() const
{
	return body;
}

