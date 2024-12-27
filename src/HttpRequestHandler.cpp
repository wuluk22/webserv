# include "HttpRequestHandler.hpp"
# include "RequestResponseState.hpp"
# include <stdlib.h>

// TO CLEAN

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

void HttpRequestHandler::reset()
{
    this->allowedPaths.clear();
    this->allowedMethods.clear();
	this->allowedPath.clear();
}

HttpRequestHandler	HttpRequestHandler::handleConfig(HttpRequestHandler& request, std::vector<LocationBlock *> locationsBlock)
{
	HttpRequestHandler tmpRequest(request);
	std::map<std::string, std::map<std::string, std::vector<std::string> > > locInfo;

	std::string					root;
	std::vector<std::string>	cgiPath;
    unsigned int                maxBody;
    bool                        autoIndex;

	root = request.getPath();
    for (std::vector<LocationBlock*>::const_iterator it = locationsBlock.begin(); it != locationsBlock.end(); ++it)
    {
        const std::string& locationUri = (*it)->getUri();

        if (locInfo.find(locationUri) == locInfo.end())
        {
            locInfo[locationUri] = std::map<std::string, std::vector<std::string> >();
        }
        if ((*it)->isGetAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "GET") == locInfo[locationUri]["allowed_methods"].end())
        {
            locInfo[locationUri]["allowed_methods"].push_back("GET");
        }
        if ((*it)->isPostAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "POST") == locInfo[locationUri]["allowed_methods"].end())
        {
            locInfo[locationUri]["allowed_methods"].push_back("POST");
        }
        if ((*it)->isDeleteAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "DELETE") == locInfo[locationUri]["allowed_methods"].end())
        {
            locInfo[locationUri]["allowed_methods"].push_back("DELETE");
        }
        if ((*it)->isCgiAllowed() && std::find(_CgiPath.begin(), _CgiPath.end(), (*it)->getCgiPath()) == _CgiPath.end())
        {
            cgiPath.push_back((*it)->getCgiPath());
        }
        if ((*it)->getAutoIndex())
        {
            locInfo[locationUri]["auto_index"].push_back("on");
            autoIndex = (*it)->getAutoIndex();
        }
        if ((*it)->getClientMaxBodySize() && std::find(locInfo[locationUri]["max_body"].begin(), locInfo[locationUri]["max_body"].end(), request.toString((*it)->getClientMaxBodySize())) == locInfo[locationUri]["max_body"].end())
        {
            locInfo[locationUri]["max_body"].push_back(request.toString((*it)->getClientMaxBodySize()));
            maxBody = (*it)->getClientMaxBodySize();
        }
        if (!(*it)->getRoot().empty())
        {
            locInfo[locationUri]["content_path"].push_back((*it)->getRoot());
        }

        std::vector<std::string> ind;
        
        ind = (*it)->accessibleIndex();
        if (!ind.empty())
            locInfo[locationUri]["index"] = ind;
    }


    if (locInfo.empty())
    {
        std::vector<std::string> emptyMethods;
        tmpRequest.setAllowedMethods(emptyMethods);
        tmpRequest.setRootDirectory(root);
        tmpRequest.setCgiPath(cgiPath);
        return tmpRequest;
    }

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator it = locInfo.begin(); it != locInfo.end(); ++it)
    {
        it->second["root_directory"].push_back(root);
    }
    tmpRequest.setCgiPath(cgiPath);
    tmpRequest.setLocInfo(locInfo);
    tmpRequest.setAutoIndex(autoIndex);
    tmpRequest.setMaxBody(maxBody);
    return tmpRequest;
}



HttpRequestHandler	HttpRequestHandler::handleRequest(int clientSock, RRState& rrstate)
{
    const size_t        bufferSize = 1024;
    char                buffer[bufferSize];
    std::string         requestData;
    HttpRequestHandler  request;
    bool                headersComplete = false;
    unsigned int        contentLength = 0;
    unsigned int        bodyLength = 0;

	request.reset();
	request.setIsValid(false);
	request = request.handleConfig(request, rrstate.getServer().getLocations());
	request.setClientSocket(clientSock);
    while (true)
	{
		request.setIsComplete(false);
        int bytesRead = recv(clientSock, buffer, bufferSize - 1, 0);
        if (bytesRead <= 0)
		{
            request.setFd(bytesRead);
            return request;
        }
        buffer[bytesRead] = '\0';
        requestData.append(buffer, bytesRead);
        if (!headersComplete)
		{
            std::string::size_type headerEnd = requestData.find("\r\n\r\n");
            
            if (headerEnd != std::string::npos)
			{
                headersComplete = true;
                std::string headersPart = requestData.substr(0, headerEnd);
                HttpRequestHandler tempRequest = httpParsing(headersPart);
                std::string contentLengthStr = tempRequest.getHeader("Content-Length");
                if (!contentLengthStr.empty())
				{
                    std::istringstream iss(contentLengthStr);
                    iss >> contentLength;
                }
                bodyLength = static_cast<unsigned int>(requestData.length() - (headerEnd + 4));
                if (contentLength == 0)
				{
                    request = httpParsing(requestData);
                    request.setFd(1);
					request.setIsComplete(true);
					request = request.handleConfig(request, rrstate.getServer().getLocations());
                    return request;
                }
            }
        }
		else
		{
			std::string::size_type headerEnd = requestData.find("\r\n\r\n");
            bodyLength = static_cast<unsigned int>(requestData.length() - (headerEnd + 4));
        }
        bool isRequestComplete = false;
        if (headersComplete)
		{
            if (contentLength > 0)
			{
                isRequestComplete = (bodyLength >= contentLength);
				request.setIsComplete(isRequestComplete);
            }
			else
			{
                isRequestComplete = true;
				request.setIsComplete(true);
            }
        }
        if (isRequestComplete)
		{
            request = httpParsing(requestData);
            request.setFd(1);
			request.setIsComplete(isRequestComplete);
			request = request.handleConfig(request, rrstate.getServer().getLocations());
            return request;
        }
        if (static_cast<unsigned int>(bytesRead) < bufferSize - 1 && !headersComplete)
		{
            break;
        }
    }
    request = httpParsing(requestData);
    request.setFd(1);
	request.setIsComplete(isRequestComplete);
	request = request.handleConfig(request, rrstate.getServer().getLocations());
    return request;
}