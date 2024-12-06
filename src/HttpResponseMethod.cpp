#include "HttpResponseHandler.hpp"
#include "CGI/Cgi.hpp"

HttpResponseHandler HttpResponseHandler::handlePath(HttpRequestHandler& request, HttpResponseHandler& response)
{
    const std::string	staticDir = request.getRootDirectory();
    std::string			filePath;
	struct stat			pathStat;
	std::string			errorPage;
	std::string			content;

	filePath = staticDir + request.getPath();

	if (!request.isMethodAllowed(request, request.getMethod()))
	{
			setErrorResponse(request, response, 405, "Method_not_allowed");
			return response;
	}
    if (isCgiRequest(request.getPath()))
    {
		handleCGI(request, "/home/salowie/Documents/webserv/src/CGI/random_quote.py");
        setErrorResponse(request, response, 200, "CGI Braowsss");
        return response;
    }
    if (request.getMethod() == "GET")
    {
        response = handleGet(request, response);
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
		path = urlDecode(path);
		//std::cout << request << std::endl;
    	if (remove(path.c_str()) == 0) {
        	response.setStatusCode(200);
        	response.setStatusMsg("OK");
        	response.setBody("Resource deleted successfully.");
    	} else {
        	response.setStatusCode(404);
        	response.setStatusMsg("Not Found");
        	response.setBody("Resource not found.");
		}
        return response;
    }
    setErrorResponse(request, response, 405, "Method not supported");
    return response;
}

HttpResponseHandler handleGet(HttpRequestHandler& request, HttpResponseHandler& response)
{
    std::string staticDir = request.getRootDirectory();
    std::string filePath;
    std::string valid;
    struct stat pathStat;
    std::string errorPage;
    std::string content;

    filePath = staticDir + request.getPath();
	valid = "/" + staticDir + request.getPath();
    // request.isPathAllowed(request, request.getPath())
    std::cout << request << std::endl;
    std::cout << "\n----filepath: " << filePath << " static: " << staticDir << " getPath: " << request.getPath() << std::endl;
    /*if (!request.isPathAllowed(request, valid) && request.getPath() != "/")
    {
        setErrorResponse(request, response, 404, "Path not allowed");
        std::cout << "\n--- ::" << valid << std::endl;
        return response;
    }*/
    if (request.getPath() == "/")
    {
        filePath = staticDir + "/index.html";
    }
    // Basic security check to prevent directory traversal
    if (filePath.find("..") != std::string::npos)
    {
        setErrorResponse(request, response, 403, "Forbidden");
        return response;
    }
    if (!request.fileExists(filePath))
    {
        setErrorResponse(request, response, 404, "Not Found meow");
        return response;
    }
    if (request.getPath() == "/static")
	{
        std::cout << "------HERE------" << std::endl;
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
				request.handleDirectoryRequest(request.getPath(), response);
	    		return response;
	    	}
	    }
    }
    content = request.readFile(filePath);
    response.setStatusCode(200);
    response.setStatusMsg("OK");
    response.setBody(content);
    response.setHeader("Content-Type", request.getMimeType(filePath));
    response.setHeader("Content-Length", request.toString(content.length()));
    response.setHttpVersion("HTTP/1.1");
    // Add security headers
    response.setHeader("X-Content-Type-Options", "nosniff");
    response.setHeader("X-Frame-Options", "SAMEORIGIN");
    response.setHeader("X-XSS-Protection", "1; mode=block");
    //std::cout << response << std::endl;
    return response;
}

void    setErrorResponse(HttpRequestHandler& request, HttpResponseHandler& response, int statusCode, const std::string& statusMsg)
{
    response.setStatusCode(statusCode);
    response.setStatusMsg(statusMsg);
    std::string errorPage = request.createErrorPage(statusCode, statusMsg);
    response.setBody(errorPage);
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Content-Length", request.toString((errorPage.length())));
}