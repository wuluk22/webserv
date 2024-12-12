#include "HttpResponseHandler.hpp"
#include "CGI/Cgi.hpp"
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


HttpResponseHandler HttpResponseHandler::handlePath(HttpRequestHandler& request, HttpResponseHandler& response)
{
    //request.setAllowedMethods(request.getAllowedMethodsFromLoc(request.getPath()));
    //request.setAllowedPaths(request.getContentPathsFromLoc(request.getPath()));
    std::map<std::string, std::vector<std::string> > config = request.getLocInfoByUri(request);

    if (config.empty())
    {
        std::cerr << "No matching configuration found for URI: " << request.getPath() << std::endl;
        setErrorResponse(request, response, 404, "Not FFFound");
        return response;
    }

    std::cerr << "\nConfiguration for URI: " << request.getPath() << std::endl;
    for (std::map<std::string, std::vector<std::string> >::iterator it = config.begin(); it != config.end(); ++it)
    {
        std::cerr << it->first << ": ";
        for (std::vector<std::string>::iterator vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt)
        {
            std::cerr << *vecIt << " ";
        }
        std::cerr << std::endl;
    }

    std::string staticDir = request.getRootDirectoryFromLoc(request.getPath());
    std::cout << "METHOD : " << request.getMethod() << std::endl;
    if (!request.isMethodAllowedInLoc(request.getPath(), request.getMethod()))
    {
        std::cerr << "HTTP method not allowed: " << request.getMethod() << std::endl;
        setErrorResponse(request, response, 405, "Method Not Allowed");
        return response;
    }

    std::string filePath = request.getFullPathFromLoc(request.getPath(), request.getPath());

    if (!request.getIsValid())
    {
        std::cerr << "Access denied for path: " << request.getPath() << std::endl;
        setErrorResponse(request, response, 403, "Forbidden");
        return response;
    }

    if (request.getMethod() == "GET")
    {
        response = handleGet(request, response);
        return response;
    }
    else if (request.getMethod() == "POST")
    {
        request.handleFileUpload(request.getBody(), filePath, response);
        return response;
    }
    else if (request.getMethod() == "DELETE")
    {
        std::string decodedPath = urlDecode(filePath);
        if (remove(decodedPath.c_str()) == 0)
        {
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody("Resource deleted successfully.");
        }
        else
        {
            setErrorResponse(request, response, 404, "Not Fwaound");
        }
        return response;
    }

    setErrorResponse(request, response, 405, "Method not supported");
    return response;
}


HttpResponseHandler handleGet(HttpRequestHandler& request, HttpResponseHandler& response)
{
    std::string staticDir = "public";
    std::string filePath;
    std::string valid;
    struct stat pathStat;
    std::string errorPage;
    std::string content;

    filePath = staticDir + request.getPath();
	valid = "/" + staticDir + request.getPath();
    // request.isPathAllowed(request, request.getPath())
    std::cout << request << std::endl;
    std::cout << "\n----filepath: " << filePath << " static: " << staticDir << " getPath: " << request.getPath() << std::endl;
    /*if (!request.isPathAllowed(request, valid) && request.getPath() != "/")
    {
        setErrorResponse(request, response, 404, "Path not allowed");
        std::cout << "\n--- ::" << valid << std::endl;
        return response;
    }*/
    if (request.getPath() == "/")
    {
        filePath = staticDir + "/index.html";
    }
    if (request.getPath() == "/docs.html")
    {
        filePath = staticDir + "/docs.html";
    }
    if (request.getPath() == "/upload.html")
    {
        filePath = staticDir + "/upload.html";
        std::cout << "--------filUUUUU : " << filePath << std::endl;
    }

    if (request.getPath() == "/favicon.ico")
    {
        std::string faviconPath = staticDir + "/favicon.ico";
        if (request.fileExists(faviconPath))
        {
            std::string content = request.readFile(faviconPath);
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody(content);
            response.setHeader("Content-Type", "image/x-icon");
            response.setHeader("Content-Length", request.toString(content.length()));
            return response;
        }
        else
        {
            setErrorResponse(request, response, 404, "Favicon Not Found");
            return response;
        }
    }

    // Basic security check to prevent directory traversal
    if (filePath.find("..") != std::string::npos)
    {
        setErrorResponse(request, response, 403, "For bidden");
        return response;
    }
    if (!request.fileExists(filePath))
    {
        std::cout << filePath << "-----------------------------------here" << std::endl;
        setErrorResponse(request, response, 404, "Not Found meow");
        return response;
    }
    if (request.getPath() == "/static")
	{
        std::cout << "------HERE------" << std::endl;
        if (stat(filePath.c_str(), &pathStat) == 0)
	    {
	    	if (S_ISDIR(pathStat.st_mode))
	    	{
				request.handleDirectoryRequest(request.getPath(), response);
	    		return response;
	    	}
	    }
    }
	/*if (request.getPath().find("/cgi-bin") == 0)
	{
		return response;
	}	*/
    if (isCgiRequest(request.getPath()))
    {
		//handleCgi(request, response);
        //setErrorResponse(request, response, 200, "CGI Braowsss");
		filePath = staticDir + "/cgi.html";
		std::cout << "CGIIIIII" << std::endl;
    }
    content = request.readFile(filePath);
    response.setStatusCode(200);
    response.setStatusMsg("OK");
    response.setBody(content);
    response.setHeader("Content-Type", request.getMimeType(filePath));
    response.setHeader("Content-Length", request.toString(content.length()));
    response.setHttpVersion("HTTP/1.1");
    // Add security headers
    response.setHeader("X-Content-Type-Options", "nosniff");
    response.setHeader("X-Frame-Options", "SAMEORIGIN");
    response.setHeader("X-XSS-Protection", "1; mode=block");
    std::cout << "-------------------RESPONSE-----------------" << std::endl;
    std::cout << response << std::endl;
    std::cout << "-------------------RESPONSE-----------------" << std::endl;
    return response;
}

void    setErrorResponse(HttpRequestHandler& request, HttpResponseHandler& response, int statusCode, const std::string& statusMsg)
{
    response.setStatusCode(statusCode);
    response.setStatusMsg(statusMsg);
    std::string errorPage = request.createErrorPage(statusCode, statusMsg);
    response.setBody(errorPage);
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Content-Length", request.toString((errorPage.length())));
}


/*void handleCgi(HttpRequestHandler& request, HttpResponseHandler& response) {
    std::string staticDir = request.getRootDirectory();
    std::string scriptPath = staticDir + request.getPath();

    // Prepare CGI environment variables
    std::vector<char*> envp;
    std::vector<std::string> envStrings;

    // Standard CGI environment variables
    envStrings.push_back("SERVER_SOFTWARE=WebServer/1.0");
    //envStrings.push_back("SERVER_NAME=" + request.getHost());
    envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envStrings.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envStrings.push_back("REQUEST_METHOD=" + request.getMethod());
    //envStrings.push_back("QUERY_STRING=" + request.getQueryString());
    envStrings.push_back("SCRIPT_NAME=" + request.getPath());
    envStrings.push_back("SCRIPT_FILENAME=" + scriptPath);
    envStrings.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
    
    // Convert content length to string
    std::ostringstream contentLengthStream;
    contentLengthStream << request.getBody().length();
    envStrings.push_back("CONTENT_LENGTH=" + contentLengthStream.str());

    // Convert environment strings to char* for execve
    for (std::vector<std::string>::iterator it = envStrings.begin(); it != envStrings.end(); ++it) {
        envp.push_back(const_cast<char*>(it->c_str()));
    }
    envp.push_back(NULL);

    // Prepare arguments
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(scriptPath.c_str()));
    argv.push_back(NULL);

    // Pipe for communication
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        setErrorResponse(request, response, 500, "Internal Server Error - Pipe creation failed");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        setErrorResponse(request, response, 500, "Internal Server Error - Fork failed");
        return;
    }

    if (pid == 0) {  // Child process
        close(pipefd[0]);
        
        // Redirect stdout to pipe write end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Execute the CGI script
        execve(scriptPath.c_str(), argv.data(), envp.data());
        
        // If execve fails
        exit(1);
    }

    // Parent process
    close(pipefd[1]);

    // Read CGI script output
    std::stringstream output;
    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        output.write(buffer, bytesRead);
    }
    close(pipefd[0]);

    // Wait for child process to finish
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        std::string outputStr = output.str();
        
        // Parse headers and body
        std::string::size_type headerEnd = outputStr.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            std::string headers = outputStr.substr(0, headerEnd);
            std::string body = outputStr.substr(headerEnd + 4);

            // Parse and set CGI headers
            std::istringstream headerStream(headers);
            std::string headerLine;
            while (std::getline(headerStream, headerLine)) {
                std::string::size_type colonPos = headerLine.find(": ");
                if (colonPos != std::string::npos) {
                    std::string headerName = headerLine.substr(0, colonPos);
                    std::string headerValue = headerLine.substr(colonPos + 2);
                    // Remove trailing \r if present
                    if (!headerValue.empty() && headerValue[headerValue.length() - 1] == '\r') {
                        headerValue = headerValue.substr(0, headerValue.length() - 1);
                    }
                    response.setHeader(headerName, headerValue);
                }
            }

            // Set body and status
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody(body);
        } else {
            // If no headers found, treat whole output as body
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody(outputStr);
        }
    } else {
        setErrorResponse(request, response, 500, "CGI Script Execution Failed");
    }
}*/
