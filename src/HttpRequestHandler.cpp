#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

int HttpRequestHandler::handleRequest(int client_sock)
{
    const size_t	bufferSize = 1024;
    bool			headersComplete = false;
    char			buffer[bufferSize];
    size_t			contentLength = 0;
	size_t			bodyLength;
	size_t			totalSent;
	ssize_t			sent;
	ssize_t			bytesRead;
    std::string		requestData;
	std::string		contentLengthStr;
	std::string		responseStr;
    
    // Read the complete request
	bytesRead = recv(client_sock, buffer, bufferSize - 1, 0);
	if (bytesRead <= 0)
	{
			// close(client_sock);
			return bytesRead;
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
                    // C++98 compatible string to unsigned long conversion
                    std::istringstream iss(contentLengthStr);
                    iss >> contentLength;
                }
            }
        }   
        // For POST requests, keep reading until we get all the content
        if (headersComplete && contentLength > 0)
        {
            std::string::size_type header_end = requestData.find("\r\n\r\n");
            bodyLength = requestData.length() - (header_end + 4);
            if (bodyLength >= contentLength)
            {
                return 2;
            }
        }
    try
    {
        HttpRequestHandler request = httpParsing(requestData);
        std::cout << request << std::endl;
        HttpResponseHandler response;
        // Handle the request
        response = response.handlePath(request, response);
        // Send response
        responseStr = response.getAll();
        totalSent = 0;
        while (totalSent < responseStr.length())
        {
            sent = send(client_sock, 
                              responseStr.c_str() + totalSent, 
                              responseStr.length() - totalSent, 
                              0);
            if (sent <= 0) 
                break;
            totalSent += sent;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error handling request: " << e.what() << std::endl;
        // Send 500 Internal Server Error
        HttpResponseHandler error_response;
        error_response.setHttpVersion("HTTP/1.1");
        error_response.setStatusCode(500);
        error_response.setStatusMsg("Internal Server Error");
        error_response.setHeader("Content-Type", "text/plain");
        // C++98 compatible integer to string conversion
        std::ostringstream oss;
        oss << strlen(e.what());
        error_response.setHeader("Content-Length", oss.str());
        error_response.setBody(e.what());

        responseStr = error_response.getAll();
        send(client_sock, responseStr.c_str(), responseStr.length(), 0);
    }
    // close(client_sock);
	return 1;
}

