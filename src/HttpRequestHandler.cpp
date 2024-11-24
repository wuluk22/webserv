#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

HttpRequestHandler HttpRequestHandler::handleRequest(int client_sock)
{
    const size_t bufferSize = 1024;
    bool headersComplete = false;
    char buffer[bufferSize];
    size_t contentLength = 0;
    size_t bodyLength = 0;
    ssize_t bytesRead;
    std::string requestData;
    std::string contentLengthStr;
    HttpRequestHandler request;

    // Read the complete request
	while (true)
	{
    	bytesRead = recv(client_sock, buffer, bufferSize - 1, 0);
    	if (bytesRead <= 0)
    	{
        	// close(client_sock);
        	request.setFd(bytesRead);
        	return request;
    	}

    	buffer[bytesRead] = '\0';
    	requestData.append(buffer, bytesRead);


    	if (!headersComplete)
    	{
        	// Find the end of headers
        	std::string::size_type header_end = requestData.find("\r\n\r\n");
        	if (header_end != std::string::npos)
        	{
            	headersComplete = true;

            	// Parse the headers to get Content-Length
            	HttpRequestHandler temp_request = httpParsing(requestData.substr(0, header_end));
            	contentLengthStr = temp_request.getHeader("Content-Length");

            	if (!contentLengthStr.empty())
            	{
                	std::istringstream iss(contentLengthStr); // C++98-compatible conversion
                	iss >> contentLength;
            	}

        		bodyLength = static_cast<unsigned int>(requestData.length() - (header_end + 4));

				if (contentLength == 0)
				{
					request = httpParsing(requestData);
					request.setFd(1);
					return request;
				}
        	}
    	}
		else
		{		
        	std::string::size_type header_end = requestData.find("\r\n\r\n");
        	bodyLength = static_cast<unsigned int>(requestData.length() - (header_end + 4));
		}
    	//std::cout << "?" << requestData << "?" << std::endl;

    	// For POST requests, keep reading until we get all the content
    	if (headersComplete && contentLength > 0 && bodyLength >= contentLength)
    	{
			request = httpParsing(requestData);
            request.setFd(1); // Content fully received
            //std::cout << "?" << request << "?" << std::endl;
            return request;
    	}

		if (static_cast<unsigned int>(bytesRead) < bufferSize - 1)
		{
			break;
		}
	}

    request = httpParsing(requestData);
    request.setFd(1);
    //std::cout << "?" << request << "?" << std::endl;
    return request;
}
