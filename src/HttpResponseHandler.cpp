#include "HttpResponseHandler.hpp"

std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i)
{
	out << i.getHttpVersion() << " " << i.getStatusCode() << " " << i.getStatusMsg() << std::endl;
	std::map<std::string, std::string> headers = i.getHeaders();
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		out << it->first << ": " << it->second << std::endl;
	}
	out << i.getBody();
	return out;
}

void	HttpResponseHandler::setHttpVersion(std::string version)
{
	this->httpVersion = version;
}

void	HttpResponseHandler::setStatusCode(int code)
{
	this->code = code;
}

void	HttpResponseHandler::setStatusMsg(std::string message)
{
	this->status = message;
}

void	HttpResponseHandler::setHeader(const std::string &headerName, const std::string &headerValue)
{
	headers[headerName] = headerValue;
}

void	HttpResponseHandler::setBody(std::string body)
{
	this->body = body;
}

std::string HttpResponseHandler::getHttpVersion() const
{
	return httpVersion;
}

int		HttpResponseHandler::getStatusCode() const
{
	return code;
}

std::string HttpResponseHandler::getStatusMsg() const
{
	return status;
}

std::string HttpResponseHandler::getHeader(const std::string &headerName) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
	if (it != headers.end())
	{
		return it->second;
	}
	return "";
}

std::map<std::string, std::string> HttpResponseHandler::getHeaders() const
{
	return headers;
}

std::string	HttpResponseHandler::getBody() const
{
	return body;
}

std::string	HttpResponseHandler::getAll() const
{
	std::ostringstream all;
	all << httpVersion << " " << code << " " << status << "\r\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		all << it->first << ": " << it->second << "\r\n";
	}
	all << "\r\n";
	all << body;
	return all.str();
}


