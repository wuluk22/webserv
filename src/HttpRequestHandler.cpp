#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

HttpRequestHandler HttpRequestHandler::handleRequest(int client_sock) {
    const size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string requestData;
    HttpRequestHandler request;
    
    // State tracking
    bool headersComplete = false;
    unsigned int contentLength = 0;
    unsigned int bodyLength = 0;
	int	i = 0;
    
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
					std::cout << " a! " << request.getIsComplete() << " a! " << std::endl;
					std::cout << " a! " << request << " a! " << std::endl;
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
			std::cout << " b! " << request.getIsComplete() << " b! " << std::endl;
			std::cout << " b! " << request << " b! " << std::endl;
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
	std::cout << " c! " << request.getIsComplete() << " c! " << std::endl;
    return request;
}
