#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

void HttpRequestHandler::reset()
{
	std::cerr << "\n--------cleaner------" << std::endl;

    this->allowedPaths.clear();
	for (std::vector<std::string>::iterator it = this->allowedMethods.begin(); it != this->allowedMethods.end(); ++it)
	{
		std::cerr << "method cleaner: " << *it << std::endl;
	}
    this->allowedMethods.clear();
	this->allowedPath.clear();
    

    /*this->rootDirectory.clear();
    this->path.clear();
    this->method.clear();
    this->httpVersion.clear();
    this->body.clear();
    

    this->headers.clear();
    this->statusCode = 0;
    this->cgiEnabled = false;*/
	std::cerr << "\n--------cleaner------" << std::endl;
}

HttpRequestHandler HttpRequestHandler::handleConfig(HttpRequestHandler& request, std::vector<LocationBlock*> locationsBlock)
{
    request.reset();
    HttpRequestHandler tmpRequest(request);
    tmpRequest.reset();
    std::map<std::string, std::map<std::string, std::vector<std::string> > > locInfo;
    std::string root = "public";

    for (std::vector<LocationBlock*>::const_iterator it = locationsBlock.begin(); it != locationsBlock.end(); ++it)
    {
        const std::string& locationUri = (*it)->getUri();

        // Check if the location matches the request path
        // Initialize the inner map for this URI if not already present
        if (locInfo.find(locationUri) == locInfo.end())
        {
            locInfo[locationUri] = std::map<std::string, std::vector<std::string> >();
        }
        // Add allowed methods
        if ((*it)->isGetAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "GET") == locInfo[locationUri]["allowed_methods"].end())
            locInfo[locationUri]["allowed_methods"].push_back("GET");
        if ((*it)->isPostAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "POST") == locInfo[locationUri]["allowed_methods"].end())
            locInfo[locationUri]["allowed_methods"].push_back("POST");
        if ((*it)->isDeleteAllowed() && std::find(locInfo[locationUri]["allowed_methods"].begin(), locInfo[locationUri]["allowed_methods"].end(), "DELETE") == locInfo[locationUri]["allowed_methods"].end())
            locInfo[locationUri]["allowed_methods"].push_back("DELETE");
        // Add content path
        if (!(*it)->getContentPath().empty())
        {
            locInfo[locationUri]["content_path"].push_back((*it)->getContentPath());
        }
    }

    if (locInfo.empty())
    {
        // No matching locations
        std::cout << "\n------meowwww-----" << std::endl;
        std::vector<std::string> emptyMethods;
        tmpRequest.setAllowedMethods(emptyMethods);
        tmpRequest.setRootDirectory(root);
        return tmpRequest;
    }

    // Add root directory to all matching URIs
    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator it = locInfo.begin(); it != locInfo.end(); ++it)
    {
        it->second["root_directory"].push_back(root);
    }

    // Debugging print to verify contents of locInfo
    std::cerr << "LocInfo Map Contents (Multiple Matches):" << std::endl;
    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator it = locInfo.begin(); it != locInfo.end(); ++it)
    {
        std::cerr << "URI: " << it->first << std::endl;
        for (std::map<std::string, std::vector<std::string> >::iterator innerIt = it->second.begin(); innerIt != it->second.end(); ++innerIt)
        {
            std::cerr << "  " << innerIt->first << ": ";
            for (std::vector<std::string>::iterator vecIt = innerIt->second.begin(); vecIt != innerIt->second.end(); ++vecIt)
            {
                std::cerr << *vecIt << " ";
            }
            std::cerr << std::endl;
        }
    }

    tmpRequest.setLocInfo(locInfo); // Assuming a setLocInfo function exists
    return tmpRequest;
}




HttpRequestHandler	HttpRequestHandler::handleRequest(int clientSock, std::vector<LocationBlock *> *locationsBlock)
{
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    HttpRequestHandler request;
    
    // State tracking
    bool headersComplete = false;
    unsigned int contentLength = 0;
    unsigned int bodyLength = 0;
	request.reset();
	request.setIsValid(false);
	//request = request.handleConfig(request, locationsBlock);
    while (true)
	{
		request.setIsComplete(false);
        int bytesRead = recv(client_sock, buffer, bufferSize - 1, 0);
        i++;
		std::cout << " MEOW " << i << " MEOW " << std::endl;
        if (bytesRead <= 0) {
            request.setFd(bytesRead);
            return request;
        }
        
        buffer[bytesRead] = '\0';
        requestData.append(buffer, bytesRead);
        
        // If headers aren't complete yet, try to find the end of headers
        if (!headersComplete) {
            std::string::size_type headerEnd = requestData.find("\r\n\r\n");
            
            if (headerEnd != std::string::npos) {
                headersComplete = true;
                
                // Get headers portion only
                std::string headersPart = requestData.substr(0, headerEnd);
                HttpRequestHandler tempRequest = httpParsing(headersPart);
                std::string contentLengthStr = tempRequest.getHeader("Content-Length");
                
                // Get content length if it exists
                if (!contentLengthStr.empty()) {
                    std::istringstream iss(contentLengthStr);
                    iss >> contentLength;
                }
                
                // Calculate current body length
                bodyLength = static_cast<unsigned int>(requestData.length() - (headerEnd + 4));
                
                // If it's a GET request or has no content length, we can parse immediately
                if (contentLength == 0) {
                    request = httpParsing(requestData);
                    request.setFd(1);
					request.setIsComplete(true);
					request = request.handleConfig(request, *locationsBlock);
					//std::cout << " a! " << request.getIsComplete() << " a! " << std::endl;
					//std::cout << " \na! " << request << " a! " << std::endl;
                    return request;
                }
            }
        } else {
            // Headers are complete, update body length
            std::string::size_type headerEnd = requestData.find("\r\n\r\n");
            bodyLength = static_cast<unsigned int>(requestData.length() - (headerEnd + 4));
        }
        
        // Check if we have received all the content
        bool isRequestComplete = false;
        if (headersComplete) {
            if (contentLength > 0) {
                // For requests with Content-Length
                isRequestComplete = (bodyLength >= contentLength);
				request.setIsComplete(isRequestComplete);
            } else {
                // For requests without Content-Length (like GET requests)
                isRequestComplete = true;
				request.setIsComplete(true);
            }
        }
        
        // Parse only when we have the complete request
        if (isRequestComplete) {
            request = httpParsing(requestData);
            request.setFd(1);
			request.setIsComplete(isRequestComplete);
			request = request.handleConfig(request, *locationsBlock);
			//std::cout << " b! " << request.getIsComplete() << " b! " << std::endl;
			//std::cout << " \nb! " << request << " b! " << std::endl;
            return request;
        }
        
        // Break if we received less than buffer size (likely end of transmission)
        // but only if we don't know we're waiting for more content
        if (static_cast<unsigned int>(bytesRead) < bufferSize - 1 && !headersComplete) {
            break;
        }
    }
    
    request = httpParsing(requestData);
    request.setFd(1);
	request.setIsComplete(isRequestComplete);
	request = request.handleConfig(request, *locationsBlock);
	//std::cout << " \nc! " << request.getIsComplete() << " c! " << std::endl;
    return request;
}


