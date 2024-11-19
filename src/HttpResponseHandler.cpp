#include "HttpResponseHandler.hpp"

HttpResponseHandler HttpResponseHandler::handlePath(HttpRequestHandler& request, HttpResponseHandler& response)
{
    const std::string	staticDir = "public";
    std::string			filePath;
	struct stat			pathStat;
	std::string			errorPage;
	std::string			content;

	filePath = staticDir + request.getPath();
	// Handle file upload
	if (request.getMethod() == "POST")
	{
		request.handleFileUpload(request.getBody(), request.getPath(), response);
		return response;
	}
	// Check if path is a directory
    if (request.getPath() == "/static")
	{
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
				request.handleDirectoryRequest(request.getPath(), response);
	    		return response;
	    	}
	    }
    }
    // Handle default page
    if (request.getPath() == "/")
    {
        filePath = staticDir + "/index.html";
    }
    // Basic security check to prevent directory traversal
    if (filePath.find("..") != std::string::npos)
    {
        response.setStatusCode(403);
        response.setStatusMsg("Forbidden");
        errorPage = request.createErrorPage(403, "Forbidden");
        response.setBody(errorPage);
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", request.toString(errorPage.length()));
        return response;
    }
    if (!request.fileExists(filePath))
    {
        response.setStatusCode(404);
        response.setStatusMsg("Not Found");
        errorPage = request.createErrorPage(404, "Not Found");
        response.setBody(errorPage);
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", request.toString(errorPage.length()));
    }
    else
    {
        content = request.readFile(filePath);
        response.setStatusCode(200);
        response.setStatusMsg("OK");
        response.setBody(content);
        response.setHeader("Content-Type", request.getMimeType(filePath));
        response.setHeader("Content-Length", request.toString(content.length()));
    }
    response.setHttpVersion("HTTP/1.1");
    // Add security headers
    response.setHeader("X-Content-Type-Options", "nosniff");
    response.setHeader("X-Frame-Options", "SAMEORIGIN");
    response.setHeader("X-XSS-Protection", "1; mode=block");
    std::cout << response << std::endl;
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


