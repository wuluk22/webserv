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

HttpResponseHandler handleGet(HttpRequestHandler& request, HttpResponseHandler& response)
{
    std::string staticDir = "public";
    std::string filePath;
    struct stat pathStat;
    std::string errorPage;
    std::string content;

	filePath = staticDir + request.getPath();
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
        setErrorResponse(request, response, 403, "Forbidden");
    if (!request.fileExists(filePath))
        setErrorResponse(request, response, 404, "Not Found meow");
    
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

void    setErrorResponse(HttpRequestHandler& request, HttpResponseHandler& response, int statusCode, const std::string& statusMsg)
{
    std::cout << " here ! " << std::endl;
    response.setStatusCode(statusCode);
    response.setStatusMsg(statusMsg);
    std::string errorPage = request.createErrorPage(statusCode, statusMsg);
    response.setBody(errorPage);
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Content-Length", request.toString((errorPage.length())));
}