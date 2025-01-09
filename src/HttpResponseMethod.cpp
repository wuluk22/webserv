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

    std::cout << "------PATH OF FILE: \n" << rrstate.getResponse().getPathOfFile(rrstate) << std::endl;

    if (config.empty())
    {
        setErrorResponse(rrstate, 404, "Not FFFound");
        return rrstate.getResponse();
    }
    std::string alles = rrstate.getRequest().getContentPath(config);
    //std::cout << "Path Handler : " << alles << std::endl;
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
        	rrstate.getResponse().setStatusMsg("Noot Found");
        	rrstate.getResponse().setBody("Resource not found.");
		}
        return rrstate.getResponse();
    }
    setErrorResponse(rrstate, 405, "Method not supported");
    return rrstate.getResponse();
}

// REFACTOR INCOMING

HttpResponseHandler HttpResponseHandler::handleGet(RRState& rrstate)
{   
    //std::cout << "Computed path : " << rrstate.getResponse().urlDecode(rrstate.getRequest().getPath()) << "\n";
    rrstate.getRequest().setPath(rrstate.getResponse().urlDecode(rrstate.getRequest().getPath()));
    std::string         staticDir = rrstate.getRequest().getRootDirectoryFromLoc(rrstate, rrstate.getRequest().getPath());
    std::string         filePath;
    struct stat         pathStat;
    std::string         errorPage;
    std::string         content;
    DirectoryHandler    hdl;
    unsigned int    max = rrstate.getRequest().getMaxBodyFromLoc(rrstate, rrstate.getRequest().getPath());
    filePath = rrstate.getRequest().getContPath();
    LocationBlock* locationBlock = rrstate.getRequest().getLocationBlock(rrstate, rrstate.getServer().getLocations());
    if (!locationBlock) {
        setErrorResponse(rrstate, 404, "Not Founnd");
        return rrstate.getResponse();
    }
    std::pair<std::string, e_data_reach>    indexResult = locationBlock->checkAvailableIndex(rrstate);
    std::string test = hdl.getMimeType(indexResult.first);
    if (((isCgiRequest(rrstate, rrstate.getRequest().getPath())) ^ (test != "text/html" || locationBlock->getAutoIndex()) )) {
        e_data_reach data;
        data = locationBlock->isContentPathReachable();
        switch (data) 
        {
            case DATA_OK: {
                break;
            }
            case DATA_NOK: {
                setErrorResponse(rrstate, 403, "Forbidden");
                return rrstate.getResponse();
            }
            case NO_DATA: {
                setErrorResponse(rrstate, 404, "Not Foundd");
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
                setErrorResponse(rrstate, 404, "NNot Found");
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
    if (filePath.find("..") != std::string::npos)
    {
        setErrorResponse(rrstate, 403, "Forbidden");
        return rrstate.getResponse();
    }
    bool fileExists = rrstate.getRequest().fileExists(filePath + rrstate.getRequest().getPath());
    if (!rrstate.getRequest().fileExists(filePath + rrstate.getRequest().getPath()) && !isCgiRequest(rrstate, rrstate.getRequest().getPath()))
    {
        setErrorResponse(rrstate, 404, "Not Found meow");
        return rrstate.getResponse();
    }
    if (isCgiRequest(rrstate, rrstate.getRequest().getPath()))
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
                setErrorResponse(rrstate, 404, "Nooot Found");
                return rrstate.getResponse();
            }
        }
        for (std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); it++) {
            path = *it;
        }
        cgi.handleCGI(rrstate, path);
        return rrstate.getResponse();
    }
    // Fautif
    indexResult = locationBlock->checkAvailableIndex(rrstate);
    filePath = indexResult.first;
    //std::cout << "Given file path: "<< filePath << std::endl;
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
    //std::cout << "yoo : " << rrstate.getRequest() << std::endl;

    return rrstate.getResponse();
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>

void setErrorResponse(RRState& rrstate, int statusCode, const std::string& statusMsg)
{
    std::map<unsigned int, std::string> meow = rrstate.getServer().getErrors();
    std::map<unsigned int, std::string>::const_iterator it = meow.find(statusCode);
    if (it != meow.end())
    {
        const std::string& errorFilePath = it->second;
        std::ifstream errorFile(errorFilePath.c_str());
        if (errorFile.is_open())
        {
            errorFile.seekg(0, std::ios::end);
            std::streampos fileSize = errorFile.tellg();
            errorFile.seekg(0, std::ios::beg);
            std::ostringstream buffer;
            buffer << errorFile.rdbuf();
            std::string errorPageContent = buffer.str();
            rrstate.getResponse().setStatusCode(statusCode);
            rrstate.getResponse().setStatusMsg(statusMsg);
            rrstate.getResponse().setBody(errorPageContent);
            rrstate.getResponse().setHeader("Content-Type", "text/html");

            std::ostringstream lengthStream;
            lengthStream << errorPageContent.length();
            rrstate.getResponse().setHeader("Content-Length", lengthStream.str());
            errorFile.close();
            return;
        }
    }
    rrstate.getResponse().setStatusCode(statusCode);
    rrstate.getResponse().setStatusMsg(statusMsg);
    std::string errorPage = rrstate.getRequest().createErrorPage(statusCode, statusMsg);
    rrstate.getResponse().setBody(errorPage);
    rrstate.getResponse().setHeader("Content-Type", "text/html");

    std::ostringstream lengthStream;
    lengthStream << errorPage.length();
    rrstate.getResponse().setHeader("Content-Length", lengthStream.str());
}