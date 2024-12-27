#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "RequestResponseState.hpp"
#include "CGI/Cgi.hpp"

HttpResponseHandler HttpResponseHandler::handlePath(RRState& rrstate)
{
    std::string                                         filePath;
	struct stat                                         pathStat;
	std::string                                         errorPage;
	std::string                                         content;
    std::map<std::string, std::vector<std::string> >    config = rrstate.getRequest().getLocInfoByUri(rrstate.getRequest());
    unsigned int                                        max = rrstate.getRequest().getMaxBodyFromLoc(rrstate, rrstate.getRequest().getPath());
    if (config.empty())
    {
        setErrorResponse(rrstate, 404, "Not FFFound");
        return rrstate.getResponse();
    }
    std::string alles = rrstate.getRequest().getContentPath(config);
    rrstate.getRequest().setContentPath(alles);
    std::map<std::string, std::vector<std::string> >::const_iterator it = config.find("allowed_methods");
    if (it != config.end())
    {
        rrstate.getRequest().setAllowedMethods(it->second);
    }
    std::string staticDir = rrstate.getRequest().getRootDirectoryFromLoc(rrstate, rrstate.getRequest().getPath());
    filePath = rrstate.getRequest().getFullPathFromLoc(rrstate, rrstate.getRequest().getPath());
	if (!rrstate.getRequest().isMethodAllowed(rrstate.getRequest(), rrstate.getRequest().getMethod()))
	{
        std::cerr << "Http method not allowed: " << rrstate.getRequest().getMethod() << std::endl;
	    setErrorResponse(rrstate, 405, "Method_not_allowed");
		return rrstate.getResponse();
	}
    if (!rrstate.getRequest().getIsValid())
    {
        std::cerr << "Access denied for path: " << rrstate.getRequest().getPath() << std::endl;
        setErrorResponse(rrstate, 403, "Forbidden");
        return rrstate.getResponse();
    }
    if (rrstate.getRequest().getMethod() == "GET")
    {
        rrstate.getResponse() = handleGet(rrstate);
        return rrstate.getResponse();
    }
	if (rrstate.getRequest().getMethod() == "POST")
	{
		rrstate.getRequest().handleFileUpload(rrstate, rrstate.getRequest().getBody(), rrstate.getRequest().getPath(), rrstate.getResponse());
		return rrstate.getResponse();
	}
	if (rrstate.getRequest().getMethod() == "DELETE")
	{
		std::string path;
        std::size_t lastSlashPos = rrstate.getRequest().getPath().find_last_of('/');
        std::string fileName;
        if (lastSlashPos != std::string::npos)
        {
            fileName = rrstate.getRequest().getPath().substr(lastSlashPos + 1);
        }
        else
        {
            fileName = rrstate.getRequest().getPath();
        }
        std::string resultPath = rrstate.getRequest().getContPath() + rrstate.getRequest().getPath();
        path = urlDecode(resultPath);
    	if (remove(path.c_str()) == 0)
        {
        	rrstate.getResponse().setStatusCode(200);
        	rrstate.getResponse().setStatusMsg("OK");
        	rrstate.getResponse().setBody("Resource deleted successfully.");
    	}
        else
        {
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
    rrstate.getRequest().setPath(rrstate.getResponse().urlDecode(rrstate.getRequest().getPath()));
    std::string         staticDir = rrstate.getRequest().getRootDirectoryFromLoc(rrstate, rrstate.getRequest().getPath());
    std::string         filePath;
    std::string         valid;
    struct stat         pathStat;
    std::string         errorPage;
    std::string         content;
    DirectoryHandler    hdl;
    unsigned int    max = rrstate.getRequest().getMaxBodyFromLoc(rrstate, rrstate.getRequest().getPath());
    filePath = rrstate.getRequest().getContPath();
	valid = rrstate.getRequest().getPath();
    LocationBlock* locationBlock = rrstate.getRequest().getLocationBlock(rrstate.getServer().getLocations());
    std::pair<std::string, e_data_reach>    indexResult = locationBlock->checkAvailableIndex();
    /*if (!request.isPathAllowed(request, valid) && request.getPath() != "/")
    {
        setErrorResponse(request, response, 404, "Path not allowed");
        std::cout << "\n--- ::" << valid << std::endl;
        return response;
    }*/
    std::string test = hdl.getMimeType(rrstate.getRequest().getPath());
    if ((isCgiRequest(rrstate.getRequest().getPath())) || test != "text/html") {
        e_data_reach data;
        data = locationBlock->isContentPathReachable();
        switch (data) 
        {
            case DATA_OK: {
                break;
            }
            case DATA_NOK: {
                if (isCgiRequest(rrstate.getRequest().getPath()))
                    break;
                setErrorResponse(rrstate, 403, "Forbidden");
                return rrstate.getResponse();
            }
            case NO_DATA: {
                if (isCgiRequest(rrstate.getRequest().getPath()))
                    break;
                setErrorResponse(rrstate, 404, "Not Found");
                return rrstate.getResponse();
            }
        }
    } else {
        switch (indexResult.second)
        {
            case DATA_OK: {
                break;
            }
            case DATA_NOK: {
                setErrorResponse(rrstate, 403, "Forbidden");
                return rrstate.getResponse();
            }
            case NO_DATA: {
                setErrorResponse(rrstate, 404, "Not Found");
                return rrstate.getResponse();
            }
        }
    }
    if (rrstate.getRequest().isAutoIndexEnabledForUri(rrstate, rrstate.getRequest().getPath()))
	{
        filePath = filePath + rrstate.getRequest().getPath();
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
				rrstate.getRequest().handleDirectoryRequest(rrstate, rrstate.getRequest().getPath(), rrstate.getResponse());
	    		return rrstate.getResponse();
	    	}
	    }
    }
    if (rrstate.getRequest().getPath() == "/")
    {
        filePath = indexResult.first;
        content = rrstate.getRequest().readFile(filePath);
        if (content.length() > max)
        {
            setErrorResponse(rrstate, 413, "response too big");
            return rrstate.getResponse();
        }
        rrstate.getResponse().setStatusCode(200);
        rrstate.getResponse().setStatusMsg("OK");
        rrstate.getResponse().setBody(content);
        rrstate.getResponse().setHeader("Content-Type", rrstate.getRequest().getMimeType(filePath));
        rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(content.length()));
        rrstate.getResponse().setHttpVersion("HTTP/1.1");
        rrstate.getResponse().setHeader("X-Content-Type-Options", "nosniff");
        rrstate.getResponse().setHeader("X-Frame-Options", "SAMEORIGIN");
        rrstate.getResponse().setHeader("X-XSS-Protection", "1; mode=block");
        return rrstate.getResponse();
    }
    if (rrstate.getRequest().getPath() == "/favicon.ico")
    {
        std::string faviconPath = staticDir + "/favicon.ico";
        if (rrstate.getRequest().fileExists(faviconPath))
        {
            std::string content = rrstate.getRequest().readFile(faviconPath);
            rrstate.getResponse().setStatusCode(200);
            rrstate.getResponse().setStatusMsg("OK");
            rrstate.getResponse().setBody(content);
            rrstate.getResponse().setHeader("Content-Type", "image/x-icon");
            rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(content.length()));
            return rrstate.getResponse();
        }
        else
        {
            setErrorResponse(rrstate, 404, "Favicon Not Found");
            return rrstate.getResponse();
        }
    }
    if (filePath.find("..") != std::string::npos)
    {
        setErrorResponse(rrstate, 403, "Forbidden");
        return rrstate.getResponse();
    }
    bool fileExists = rrstate.getRequest().fileExists(filePath + rrstate.getRequest().getPath());
    if (!rrstate.getRequest().fileExists(filePath + rrstate.getRequest().getPath()) && !isCgiRequest(rrstate.getRequest().getPath()))
    {
        setErrorResponse(rrstate, 404, "Not Found meow");
        return rrstate.getResponse();
    }
    if (isCgiRequest(rrstate.getRequest().getPath()))
    {
        Cgi                                     cgi;
        std::string                             path;
        std::vector<std::string>                uris = rrstate.getRequest().getContentPathsFromLoc(rrstate, rrstate.getRequest().getPath());
        switch(locationBlock->isContentPathReachable())
        {
            case DATA_OK:
                break;
            case DATA_NOK: {
                setErrorResponse(rrstate, 403, "Forbidden");
                return rrstate.getResponse();
            }
            case NO_DATA: {
                setErrorResponse(rrstate, 404, "Not Found");
                return rrstate.getResponse();
            }
        }
        for (std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); it++) {
            path = *it;
        }
        cgi.handleCGI(rrstate, path);
        return rrstate.getResponse();
    }
    filePath = filePath + rrstate.getRequest().getPath();
    content = rrstate.getRequest().readFile(filePath);
    if (content.length() > max)
    {
        setErrorResponse(rrstate, 413, "response too big");
        return rrstate.getResponse();
    }
    rrstate.getResponse().setStatusCode(200);
    rrstate.getResponse().setStatusMsg("OK");
    rrstate.getResponse().setBody(content);
    rrstate.getResponse().setHeader("Content-Type", rrstate.getRequest().getMimeType(filePath));
    rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(content.length()));
    rrstate.getResponse().setHttpVersion("HTTP/1.1");
    rrstate.getResponse().setHeader("X-Content-Type-Options", "nosniff");
    rrstate.getResponse().setHeader("X-Frame-Options", "SAMEORIGIN");
    rrstate.getResponse().setHeader("X-XSS-Protection", "1; mode=block");
    return rrstate.getResponse();
}

void setErrorResponse(RRState& rrstate, int statusCode, const std::string& statusMsg)
{
    std::map<unsigned int, std::string>                 meow = rrstate.getServer().getErrors();
    std::map<unsigned int, std::string>::const_iterator it = meow.find(statusCode);
    if (it != meow.end())
    {
        const std::string& errorFilePath = it->second;
        std::ifstream errorFile(errorFilePath.c_str());
        if (errorFile.is_open())
        {
            std::ostringstream buffer;
            buffer << errorFile.rdbuf();
            std::string errorPageContent = buffer.str();
            rrstate.getResponse().setStatusCode(statusCode);
            rrstate.getResponse().setStatusMsg(statusMsg);
            rrstate.getResponse().setBody(errorPageContent);
            rrstate.getResponse().setHeader("Content-Type", "text/html");
            rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(errorPageContent.length()));
            errorFile.close();
            return;
        }
        else
        {
            std::cerr << "Error: Could not open error file: " << errorFilePath << std::endl;
        }
    }

    rrstate.getResponse().setStatusCode(statusCode);
    rrstate.getResponse().setStatusMsg(statusMsg);
    std::string errorPage = rrstate.getRequest().createErrorPage(statusCode, statusMsg);
    rrstate.getResponse().setBody(errorPage);
    rrstate.getResponse().setHeader("Content-Type", "text/html");
    rrstate.getResponse().setHeader("Content-Length", rrstate.getRequest().toString(errorPage.length()));
}
