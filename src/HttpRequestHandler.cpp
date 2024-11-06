#include "HttpRequestHandler.hpp"
#include <string>
#include <iostream>

const int BUFFER_SIZE = 1024;

std::ostream	&operator<<(std::ostream &out, const HttpRequestHandler &i)
{
	out << i.getMethod() << " " << i.getPath() << " " << i.getHttpVersion() << std::endl;
	std::map<std::string, std::string> headers = i.getHeaders();
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		out << it->first << ": " << it->second << std::endl;
	}
	return out;
}

std::string HttpRequestHandler::getMethod(void) const
{
	return method;
}

std::string HttpRequestHandler::getPath(void) const
{
	return path;
}

std::string HttpRequestHandler::getHttpVersion(void) const
{
	return httpVersion;
}

std::string HttpRequestHandler::getHeader(const std::string &headerName) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
    if (it != headers.end())
	{
        return it->second;
    }
    return ""; // Return empty string if header is not found
}

std::map<std::string, std::string> HttpRequestHandler::getHeaders() const
{
    return headers;
}

std::string HttpRequestHandler::getBody() const
{
    return body;
}

void	HttpRequestHandler::setMethod(const std::string &m)
{
	method = m;
}

void	HttpRequestHandler::setPath(const std::string &p)
{
	path = p;
}

void	HttpRequestHandler::setHttpVersion(const std::string &h)
{
	httpVersion = h;
}

void HttpRequestHandler::setHeader(const std::string &headerName, const std::string &headerValue)
{
    headers[headerName] = headerValue;
}

void HttpRequestHandler::setBody(const std::string &b)
{
    body = b;
}

HttpRequestHandler	HttpRequestHandler::httpParsing(const std::string &buffer)
{
	HttpRequestHandler	request;
	size_t	methodPos;
	size_t	pathPos;
	size_t	httpVersionPos;

	/*std::cout << "-----------------" << std::endl;
    std::cout << "Received request:\n" << buffer << std::endl;
	std::cout << "-----------------" << std::endl;*/

	std::istringstream	stream(buffer);
	std::string	line;
	std::getline(stream, line);
	//std::cout << line << std::endl;

	// method
	methodPos = line.find(" ");
	request.setMethod(line.substr(0, methodPos));
	
	
	// path
	pathPos = methodPos + 1;
	httpVersionPos = line.find(" ", pathPos);
	request.setPath(line.substr(pathPos, httpVersionPos - pathPos));

	// http version
	request.setHttpVersion(line.substr(httpVersionPos + 1, '\n'));

	while (std::getline(stream, line) && line != "\r" && !line.empty())
	{
		std::string	headerName;
		std::string	headerValue;
		size_t	pos;

		pos = line.find(":");
		headerName = line.substr(0, pos);
		headerValue = line.substr(pos + 2);
		request.setHeader(headerName, headerValue);
	}

    std::string bodyContent;
    while (std::getline(stream, line))
	{
        bodyContent += line + "\n";
    }
    setBody(bodyContent);


	return request;
}

void HttpRequestHandler::handle_request(int client_sock)
{
	HttpRequestHandler http;
    char buffer[BUFFER_SIZE];
    int valread = recv(client_sock, buffer, BUFFER_SIZE, 0);

    if (valread == 0)  // Client disconnected
    {
        std::cout << "Client disconnected, closing socket" << std::endl;
        close(client_sock);
        return;
    }
    else if (valread < 0)
    {
        std::cerr << "Error in receiving data" << std::endl;
        return;
    }

    buffer[valread] = '\0';

	http = http.httpParsing(buffer);
	std::cout << " -<- getters --- " << std::endl;
	std::cout << http << std::endl;
	std::cout << " --- getters ->- " << std::endl;

    // Simple HTTP Response
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n\r\n"
        "Hello, World!";

    send(client_sock, response.c_str(), response.length(), 0);
    std::cout << "Response sent to client" << std::endl;

    close(client_sock);  // Close the client socket after sending response
}

