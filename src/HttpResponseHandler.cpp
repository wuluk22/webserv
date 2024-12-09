#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"

void HttpResponseHandler::handleResponse(HttpRequestHandler& request, int clientSock)
{
    try
    {
        *this = handlePath(request, *this);

        std::string responseStr = getAll();
        size_t totalSent = 0;
        ssize_t sent;

        while (totalSent < responseStr.length())
        {
            sent = send(clientSock, 
                        responseStr.c_str() + totalSent, 
                        responseStr.length() - totalSent, 
                        0);

            if (sent <= 0)
                throw std::runtime_error("Failed to send response to client.");
            
            totalSent += sent;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error sending response: " << e.what() << std::endl;
        throw;
    }
}


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

void HttpResponseHandler::handleResponse(HttpRequestHandler& request, int client_sock)
{
    try
    {
        // Generate the appropriate response for the request
        *this = handlePath(request, *this);

        // Send the response to the client
        std::string responseStr = getAll();
        size_t totalSent = 0;
        ssize_t sent;

        while (totalSent < responseStr.length())
        {
            sent = send(client_sock, 
                        responseStr.c_str() + totalSent, 
                        responseStr.length() - totalSent, 
                        0);

            if (sent <= 0)
                throw std::runtime_error("Failed to send response to client.");
            
            totalSent += sent;
        }
    }
    catch (const std::exception& e)
    {
        // If sending fails, log the error and rethrow to the caller
        std::cerr << "Error sending response: " << e.what() << std::endl;
        throw;
    }
}

void HttpResponseHandler::sendError(int client_sock, int statusCode, const std::string& statusMsg, const std::string& body)
{
    try
    {
        // Create an error response
        setHttpVersion("HTTP/1.1");
        setStatusCode(statusCode);
        setStatusMsg(statusMsg);
        setHeader("Content-Type", "text/plain");

        std::ostringstream oss;
        oss << body.length();
        setHeader("Content-Length", oss.str());
        setBody(body);

        // Send the error response
        std::string responseStr = getAll();
        send(client_sock, responseStr.c_str(), responseStr.length(), 0);
    }
    catch (const std::exception& e)
    {
        // Log failure in sending error response
        std::cerr << "Error sending error response: " << e.what() << std::endl;
    }
}
