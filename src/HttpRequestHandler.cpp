#include "HttpRequestHandler.hpp"
#include <algorithm>

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

	bool	completed("false");
	int		i = 0;
	root = "public";

	/*std::cout << "010- " << locationsBlock[0]->getUri() << " -010-" << std::endl;
	std::cout << "020- " << locationsBlock[1]->getUri() << " -010-" << std::endl;
	

	
	std::cout << "030- " << locationsBlock[0]->isGetAllowed() << " -010-" << std::endl;
	std::cout << "040- " << locationsBlock[0]->isPostAllowed() << " -010-" << std::endl;
	std::cout << "050- " << locationsBlock[0]->isDeleteAllowed() << " -010-" << std::endl;*/

	while (i < locationsBlock.size() && locationsBlock[i])
	{
			if (locationsBlock[i]->isGetAllowed())
				if (std::find(methods.begin(), methods.end(), std::string("GET")) == methods.end())
					methods.push_back("GET");
			if (locationsBlock[i]->isPostAllowed())
				if (std::find(methods.begin(), methods.end(), std::string("POST")) == methods.end())
					methods.push_back("POST");
			if (locationsBlock[i]->isDeleteAllowed())
				if (std::find(methods.begin(), methods.end(), std::string("DELETE")) == methods.end())
					methods.push_back("DELETE");


			std::string uri = locationsBlock[i]->getUri();
			if (!uri.empty())
					paths.push_back(locationsBlock[i]->getUri());
			i++;
	}

	tmpRequest.setAllowedMethods(methods);
	tmpRequest.setAllowedPaths(paths);
	tmpRequest.setRootDirectory(root);
	

	std::cerr << "\ntest 4: \n" << tmpRequest << "end!" << std::endl;


	return tmpRequest;
}



HttpRequestHandler	HttpRequestHandler::handleRequest(int client_sock, std::vector<LocationBlock *> locationsBlock) {
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    HttpRequestHandler request;
    
    // State tracking
    bool headersComplete = false;
    unsigned int contentLength = 0;
    unsigned int bodyLength = 0;
	int	i = 0;
	
	request = request.handleConfig(request, locationsBlock);

	/*std::cout << "?!?" << allowedMethods[0] << "?!?" << std::endl;
	std::cout << "?!?" << allowedMethods[1] << "?!?" << std::endl;
	std::cout << "?!?" << allowedMethods[2] << "?!?" << std::endl;*/

    while (true) {
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
					request = request.handleConfig(request, locationsBlock);
					std::cout << " a! " << request.getIsComplete() << " a! " << std::endl;
					std::cout << " \na! " << request << " a! " << std::endl;
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
			request = request.handleConfig(request, locationsBlock);
			std::cout << " b! " << request.getIsComplete() << " b! " << std::endl;
			std::cout << " \nb! " << request << " b! " << std::endl;
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
	request = request.handleConfig(request, locationsBlock);
	std::cout << " \nc! " << request.getIsComplete() << " c! " << std::endl;
    return request;
}
