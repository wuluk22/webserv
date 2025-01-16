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
        setErrorResponse(rrstate, 404, "Not Found");
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
	    setErrorResponse(rrstate, 405, "Method_not_allowed");
		return rrstate.getResponse();
	}
    if (!rrstate.getRequest().getIsValid())
    {
        setErrorResponse(rrstate, 403, "Forbidden");
        return rrstate.getResponse();
    }
    if (rrstate.getRequest().getMethod() == "GET")
    {
        rrstate.getResponse() = handleGet(rrstate);
        if (rrstate.getRequest().getBody().length() > max)
        {
            return errorHandler(rrstate, 413, "Payload Too Large");
        }
        return rrstate.getResponse();
    }
	if (rrstate.getRequest().getMethod() == "POST")
	{
		rrstate.getRequest().handleFileUpload(rrstate, rrstate.getRequest().getBody(), rrstate.getRequest().getPath(), rrstate.getResponse());
        if (rrstate.getRequest().getBody().length() > max)
        {
            return errorHandler(rrstate, 413, "Payload Too Large");
        }
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
        if (rrstate.getRequest().getBody().length() > max)
        {
            return errorHandler(rrstate, 413, "Payload Too Large");
        }
        return rrstate.getResponse();
    }
    setErrorResponse(rrstate, 405, "Method not supported");
    if (rrstate.getRequest().getBody().length() > max)
    {
        return errorHandler(rrstate, 413, "Payload Too Large");
    }
    return rrstate.getResponse();
}

HttpResponseHandler HttpResponseHandler::errorHandler(RRState &rrstate, unsigned int error_code, std::string message)
{
    setErrorResponse(rrstate, error_code, message);
    return rrstate.getResponse();
}

std::string generateSessionId()
{
    std::srand(std::time(0));
    std::string sessionId;
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    const size_t maxLength = 16;

    for (size_t i = 0; i < maxLength; ++i)
    {
        sessionId += charset[std::rand() % (sizeof(charset) - 1)];
    }
    return sessionId;
}

HttpResponseHandler HttpResponseHandler::handleGet(RRState& rrstate) {
    HttpRequestHandler request = rrstate.getRequest();
    HttpResponseHandler response = rrstate.getResponse();
    ServerHandler server = rrstate.getServer();
    LocationBlock* l_block;
    PathValidator validator;
    std::string content_file;
    std::string content;
    unsigned int max = request.getMaxBodyFromLoc(rrstate, request.getPath());
    request.setPath(response.urlDecode(request.getPath()));
    content_file = request.getContPath() + request.getPath();
    l_block = request.getLocationBlock(rrstate, server.getLocations());
    if (!l_block) {
        return errorHandler(rrstate, 404, "Not Found");
    }
    validator.setPath(content_file);
    if (validator.exists()) {
        if (validator.isDirectory()) {
            if (request.isAutoIndexEnabledForUri(rrstate, request.getPath())) {
                request.handleDirectoryRequest(rrstate, request.getPath(), response);
                if (response.getBody().length() > max) {
                    return errorHandler(rrstate, 413, "Payload Too Large");
                }
                return rrstate.getResponse();
            } else {
                content_file = l_block->checkAvailableRessource().first;
                validator.setPath(content_file);
                if (validator.exists() && !validator.isReadable()){
                    return errorHandler(rrstate, 403, "Forbidden");
                } else if (!validator.exists() && !l_block->isCgiAllowed()){
                    return errorHandler(rrstate, 404, "Not Found");
                }
            }
        }
    } else if (validator.exists() && !validator.isReadable()){
        return errorHandler(rrstate, 403, "Forbidden");
    } else if (!validator.exists() && !l_block->isCgiAllowed() ){
        return errorHandler(rrstate, 404, "Not Found");
    }
    std::pair<std::string, e_data_reach> hihi = l_block->checkAvailableRessource();
    bool isCgi = isCgiRequest(rrstate, request.getPath());
    if (isCgi) {
        Cgi                                     cgi;
        std::string                             path;
        std::vector<std::string>                uris;

        uris = request.getContentPathsFromLoc(rrstate, request.getPath());
        switch(l_block->isContentPathReachable()) {
            case DATA_OK:
                break;
            case DATA_NOK:
                return errorHandler(rrstate, 403, "Forbidden");
            case NO_DATA:
                return errorHandler(rrstate, 404, "Not found");
        }
        for (std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); it++)
            path = *it;
        cgi.handleCGI(rrstate, l_block->getUri() ,urlDecode(path));
        if (rrstate.getResponse().getBody().length() > max)
        {
            return errorHandler(rrstate, 413, "Payload Too Large");
        }
        return (rrstate.getResponse());
    } else if (!isCgi && l_block->isCgiAllowed())
        return errorHandler(rrstate, 404, "Not found");
    content = request.readFile(rrstate, content_file);
    if (content.length() > max) {
        return errorHandler(rrstate, 413, "Payload Too Large");
    }
    response.setStatusCode(200);
    response.setStatusMsg("OK");
    response.setBody(content);
    response.setHeader("Content-Type", request.getMimeType(content_file));
    response.setHeader("Content-Length", request.toString(content.length()));
    response.setHttpVersion("HTTP/1.1");
    response.setHeader("X-Content-Type-Options", "nosniff");
    response.setHeader("X-Frame-Options", "SAMEORIGIN");
    response.setHeader("X-XSS-Protection", "1; mode=block");
    return (response);
}

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
            rrstate.getResponse().setHttpVersion("HTTP/1.1");
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
    rrstate.getResponse().setHttpVersion("HTTP/1.1");
    std::ostringstream lengthStream;
    lengthStream << errorPage.length();
    rrstate.getResponse().setHeader("Content-Length", lengthStream.str());
}
