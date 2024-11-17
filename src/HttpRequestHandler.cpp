#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler()
{}

HttpRequestHandler::~HttpRequestHandler()
{}

HttpResponseHandler HttpRequestHandler::handlePath(const HttpRequestHandler& request, HttpResponseHandler& response)
{
    const std::string	staticDir = "public";
    std::string			filePath;
	struct stat			pathStat;
	std::string			errorPage;
	std::string			content;

	filePath = staticDir + request.getPath();
	// Handle file upload
	if (request.getMethod() == "POST")
	{
		handleFileUpload(request.getBody(), request.getPath(), response);
		return response;
	}
	// Check if path is a directory
    if (request.getPath() == "/static")
	{
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
	    		handleDirectoryRequest(request.getPath(), response);
	    		return response;
	    	}
	    }
    }
    // Handle default page
    if (request.getPath() == "/")
    {
        filePath = staticDir + "/index.html";
    }
    // Basic security check to prevent directory traversal
    if (filePath.find("..") != std::string::npos)
    {
        response.setStatusCode(403);
        response.setStatusMsg("Forbidden");
        errorPage = createErrorPage(403, "Forbidden");
        response.setBody(errorPage);
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", toString(errorPage.length()));
        return response;
    }
    if (!fileExists(filePath))
    {
        response.setStatusCode(404);
        response.setStatusMsg("Not Found");
        errorPage = createErrorPage(404, "Not Found");
        response.setBody(errorPage);
        response.setHeader("Content-Type", "text/html");
        response.setHeader("Content-Length", toString(errorPage.length()));
    }
    else
    {
        content = readFile(filePath);
        response.setStatusCode(200);
        response.setStatusMsg("OK");
        response.setBody(content);
        response.setHeader("Content-Type", getMimeType(filePath));
        response.setHeader("Content-Length", toString(content.length()));
    }
    response.setHttpVersion("HTTP/1.1");
    // Add security headers
    response.setHeader("X-Content-Type-Options", "nosniff");
    response.setHeader("X-Frame-Options", "SAMEORIGIN");
    response.setHeader("X-XSS-Protection", "1; mode=block");
    std::cout << response << std::endl;
    return response;
}

void HttpRequestHandler::handleRequest(int client_sock)
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
    while (true)
    {
        bytesRead = recv(client_sock, buffer, bufferSize - 1, 0);
        if (bytesRead <= 0) 
            break;     
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
                break;
            }
        }
    }
    if (requestData.empty())
    {
        close(client_sock);
        return;
    }
    try
    {
        HttpRequestHandler request = httpParsing(requestData);
        HttpResponseHandler response;
        // Handle the request
        response = request.handlePath(request, response);
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
    close(client_sock);
}

