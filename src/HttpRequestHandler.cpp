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
    size_t bodyLength;
    ssize_t bytesRead;
    std::string requestData;
    std::string contentLengthStr;
    HttpRequestHandler request;

    // Read the complete request
    bytesRead = recv(client_sock, buffer, bufferSize - 1, 0);
    if (bytesRead <= 0)
    {
        close(client_sock);
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
        }
    }

    // For POST requests, keep reading until we get all the content
    if (headersComplete && contentLength > 0)
    {
        std::string::size_type header_end = requestData.find("\r\n\r\n");
        bodyLength = requestData.length() - (header_end + 4);

        if (bodyLength >= contentLength)
        {
            request.setFd(2); // Content fully received
            return request;
        }
    }
    try
    {
        request = httpParsing(requestData);
        HttpResponseHandler response;
        response.handleResponse(request, client_sock);
    }
    catch (const std::exception& e)
    {
        HttpResponseHandler response;
        // Send a 500 Internal Server Error using HttpResponseHandler
        response.sendError(client_sock, 500, "Internal Server Error", e.what());
    }

    close(client_sock);
    request.setFd(1);
    return request;
}

