#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

HttpRequestHandler	HttpRequestHandler::handleConfig(HttpRequestHandler& request, std::vector<LocationBlock *> locationsBlock)
{
	HttpRequestHandler tmpRequest(request);
	std::vector<std::string>	paths;
	std::vector<std::string>	methods;
	std::string					root;
	std::vector<std::string>	cgiPath;

	bool	completed("false");
	int		i = 0;
	root = "public";

	while (i < locationsBlock.size() && locationsBlock[i])
	{
			if (locationsBlock[i]->isGetAllowed())
				if (std::find(methods.begin(), methods.end(), "GET") == methods.end())
					methods.push_back("GET");
			if (locationsBlock[i]->isPostAllowed())
				if (std::find(methods.begin(), methods.end(), "POST") == methods.end())
					methods.push_back("POST");
			if (locationsBlock[i]->isDeleteAllowed())
				if (std::find(methods.begin(), methods.end(), "DELETE") == methods.end())
					methods.push_back("DELETE");
			if (locationsBlock[i]->isCgiAllowed()) {
				if (std::find(_CgiPath.begin(), _CgiPath.end(),locationsBlock[i]->getCgiPath()) == _CgiPath.end()) {
					cgiPath.push_back(locationsBlock[i]->getCgiPath());
				}
			}
			std::string uri = locationsBlock[i]->getUri();
			if (!uri.empty() && std::find(paths.begin(), paths.end(), uri) == paths.end())
					paths.push_back(locationsBlock[i]->getUri());
			/*if (block->hasRoot())
            	root = block->getRoot();
        
        	if (block->hasServerName())
        	    serverName = block->getServerName();
			
        	if (block->hasClientMaxBodySize())
        	    clientMaxBodySize = block->getClientMaxBodySize();
			
        	if (block->hasRedirect())
			{
        	    redirectEnabled = true;
        	    redirectUrl = block->getRedirectUrl();
        	}*/
			i++;
	}
	tmpRequest.setAllowedMethods(methods);
	tmpRequest.setAllowedPaths(paths);
	tmpRequest.setRootDirectory(root);
	tmpRequest.setCgiPath(cgiPath);
	//std::cerr << "\ntest 4: \n" << tmpRequest << "end!" << std::endl;
	return tmpRequest;
}



HttpRequestHandler	HttpRequestHandler::handleRequest(int clientSock, std::vector<LocationBlock *> locationsBlock)
{
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    HttpRequestHandler request;
    bool headersComplete = false;
    unsigned int contentLength = 0;
    unsigned int bodyLength = 0;
	
	request = request.handleConfig(request, locationsBlock);
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
					request = request.handleConfig(request, locationsBlock);
					//std::cout << " a! " << request.getIsComplete() << " a! " << std::endl;
					//std::cout << " \na! " << request << " a! " << std::endl;
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
			request = request.handleConfig(request, locationsBlock);
			//std::cout << " b! " << request.getIsComplete() << " b! " << std::endl;
			//std::cout << " \nb! " << request << " b! " << std::endl;
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
	request = request.handleConfig(request, locationsBlock);
	//std::cout << " \nc! " << request.getIsComplete() << " c! " << std::endl;
    return request;
}