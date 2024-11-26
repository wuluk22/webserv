#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

std::string url_decode(const std::string& url) {
    std::string decoded;
    size_t length = url.length();
    
    for (size_t i = 0; i < length; ++i) {
        if (url[i] == '%') {
            // Ensure we have two more characters for the hex code
            if (i + 2 < length) {
                // Convert the next two characters to a hex value using strtol
                std::string hex_value = url.substr(i + 1, 2);
                char decoded_char = static_cast<char>(strtol(hex_value.c_str(), NULL, 16));
                decoded += decoded_char;
                i += 2; // Skip the next two characters as they are part of the encoding
            }
        } else if (url[i] == '+') {
            // Convert '+' to a space
            decoded += ' ';
        } else {
            // Copy the character as is
            decoded += url[i];
        }
    }

    return decoded;
}


bool isCgiRequest(const std::string& path) {
    const char* cgiExtensionsArray[] = {".cgi", ".pl", ".py"};
    std::vector<std::string> cgiExtensions(cgiExtensionsArray, cgiExtensionsArray + sizeof(cgiExtensionsArray) / sizeof(cgiExtensionsArray[0]));

    const std::string cgiDirectory = "/cgi-bin";

    if (path.find(cgiDirectory) == 0) {
        return true;
    }

    /*for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
        const std::string& ext = *it;
        if (path.length() >= ext.length() &&
            path.compare(path.length() - ext.length(), ext.length(), ext) == 0) {
            struct stat fileStat;
            std::string fullPath = "cgi-bin" + path.substr(cgiDirectory.length());
            if (stat(fullPath.c_str(), &fileStat) == 0 && (fileStat.st_mode & S_IXUSR)) {
                return true;
            }
        }
    }*/
    return false;
}



HttpResponseHandler HttpResponseHandler::handlePath(HttpRequestHandler& request, HttpResponseHandler& response)
{
    const std::string	staticDir = "public";
    std::string			filePath;
	struct stat			pathStat;
	std::string			errorPage;
	std::string			content;

	filePath = staticDir + request.getPath();
	// Handle file upload
    if (isCgiRequest(request.getPath()))
    {
        std::cout << " oioi! " << std::endl;
        setErrorResponse(request, response, 200, "CGI Braowsss");
        return response;
    }
	if (request.getMethod() == "POST")
	{
		request.handleFileUpload(request.getBody(), request.getPath(), response);
		return response;
	}
	if (request.getMethod() == "DELETE")
	{
		std::string path;
		path = staticDir + request.getPath();
		path = url_decode(path);
		std::cout << " MAAA! " << std::endl;
		std::cout << request << std::endl;
    	if (remove(path.c_str()) == 0) {
        	// Si la suppression a réussi
        	response.setStatusCode(200); // OK
        	response.setStatusMsg("OK");
        	response.setBody("Resource deleted successfully.");
			std::cout << " MOOO! " << std::endl;
			return response;
    	} else {
        	// Si la suppression a échoué
        	response.setStatusCode(404); // Not Found
        	response.setStatusMsg("Not Found");
        	response.setBody("Resource not found.");
			std::cout << " Miii! " << std::endl;
			return response;
		}
    }
    if (request.getMethod() == "GET")
    {
        response = handleGet(request, response);
        return response;
    }
    /*
    else -> invalid method
    */
    return response;
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
