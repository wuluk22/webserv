#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "RequestResponseState.hpp"
#include "CGI/Cgi.hpp"

HttpResponseHandler HttpResponseHandler::handlePath(RRState& rrstate)
{
    const std::string	staticDir = rrstate.getRequest().getRootDirectory();
    std::string			filePath;
	struct stat			pathStat;
	std::string			errorPage;
	std::string			content;

	filePath = staticDir + rrstate.getRequest().getPath();

	if (!rrstate.getRequest().isMethodAllowed(rrstate.getRequest(), rrstate.getRequest().getMethod()))
	{
			setErrorResponse(rrstate, 405, "Method_not_allowed");
			return rrstate.getResponse();
	}
    if (rrstate.getRequest().getMethod() == "GET")
    {
        rrstate.getResponse() = handleGet(rrstate);
        return rrstate.getResponse();
    }
	if (rrstate.getRequest().getMethod() == "POST")
	{
		rrstate.getRequest().handleFileUpload(rrstate.getRequest().getBody(), rrstate.getRequest().getPath(), rrstate.getResponse());
		return rrstate.getResponse();
	}
	if (rrstate.getRequest().getMethod() == "DELETE")
	{
		std::string path;
		path = staticDir + rrstate.getRequest().getPath();
		path = urlDecode(path);
		//std::cout << request << std::endl;
    	if (remove(path.c_str()) == 0) {
        	rrstate.getResponse().setStatusCode(200);
        	rrstate.getResponse().setStatusMsg("OK");
        	rrstate.getResponse().setBody("Resource deleted successfully.");
    	} else {
        	rrstate.getResponse().setStatusCode(404);
        	rrstate.getResponse().setStatusMsg("Not Found");
        	rrstate.getResponse().setBody("Resource not found.");
		}
        return rrstate.getResponse();
    }
    setErrorResponse(rrstate, 405, "Method not supported");
    return rrstate.getResponse();
}

HttpResponseHandler HttpResponseHandler::handleGet(RRState& rrstate)
{
    std::string staticDir = rrstate.getRequest().getRootDirectory();
    std::string filePath;
    std::string valid;
    struct stat pathStat;
    std::string errorPage;
    std::string content;

    filePath = staticDir + rrstate.getRequest().getPath();
	valid = "/" + staticDir + rrstate.getRequest().getPath();
    // request.isPathAllowed(request, request.getPath())
    std::cout << rrstate.getRequest() << std::endl;
    std::cout << "\n----filepath: " << filePath << " static: " << staticDir << " getPath: " << rrstate.getRequest().getPath() << std::endl;
    /*if (!request.isPathAllowed(request, valid) && request.getPath() != "/")
    {
        setErrorResponse(request, response, 404, "Path not allowed");
        std::cout << "\n--- ::" << valid << std::endl;
        return response;
    }*/
    if (rrstate.getRequest().getPath() == "/")
    {
        filePath = staticDir + "/index.html";
    }
    // Basic security check to prevent directory traversal
    if (filePath.find("..") != std::string::npos)
    {
        setErrorResponse(rrstate, 403, "Forbidden");
        return rrstate.getResponse();
    }
    if (rrstate.getRequest().fileExists(filePath) && !isCgiRequest(rrstate.getRequest().getPath()))
    {
        setErrorResponse(rrstate, 404, "Not Found meow");
        return rrstate.getResponse();
    }
    if (rrstate.getRequest().getPath() == "/static")
	{
        std::cout << "------HERE------" << std::endl;
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
				rrstate.getRequest().handleDirectoryRequest(rrstate.getRequest().getPath(), rrstate.getResponse());
	    		return rrstate.getResponse();
	    	}
	    }
    }
	/*if (request.getPath().find("/cgi-bin") == 0)
	{
		return response;
	}	*/
    if (isCgiRequest(rrstate.getRequest().getPath()))
    {
        Cgi cgi;
        std::cout << "CLIENTSOCK IN RESPONSEMETHOD : " << rrstate.getRequest().getClientSocket() << std::endl;
		cgi.handleCGI(rrstate);
        //setErrorResponse(request, response, 200, "CGI Braowsss");
		filePath = staticDir + "/cgi.html";
    }
    content = rrstate.getRequest().readFile(filePath);
    rrstate.getResponse().setStatusCode(200);
    rrstate.getResponse().setStatusMsg("OK");
    rrstate.getResponse().setBody(content);
    rrstate.getResponse().setHeader("Content-Type", rrstate.getRequest().getMimeType(filePath));
    rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(content.length()));
    rrstate.getResponse().setHttpVersion("HTTP/1.1");
    // Add security headers
    rrstate.getResponse().setHeader("X-Content-Type-Options", "nosniff");
    rrstate.getResponse().setHeader("X-Frame-Options", "SAMEORIGIN");
    rrstate.getResponse().setHeader("X-XSS-Protection", "1; mode=block");
    //std::cout << response << std::endl;
    return rrstate.getResponse();
}

void    setErrorResponse(RRState& rrstate, int statusCode, const std::string& statusMsg)
{
    rrstate.getResponse().setStatusCode(statusCode);
    rrstate.getResponse().setStatusMsg(statusMsg);
    std::string errorPage = rrstate.getRequest().createErrorPage(statusCode, statusMsg);
    rrstate.getResponse().setBody(errorPage);
    rrstate.getResponse().setHeader("Content-Type", "text/html");
    rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString((errorPage.length())));
}
