HttpResponseHandler HttpResponseHandler::handlePath(HttpRequestHandler& request, HttpResponseHandler& response)
{
    // Retrieve the configuration for the request URI
    std::map<std::string, std::vector<std::string> > config = request.getLocInfoByUri(request);

    // Check if config is empty (no match found)
    if (config.empty())
    {
        std::cerr << "No matching configuration found for URI: " << request.getPath() << std::endl;
        setErrorResponse(request, response, 404, "Not Found");
        return response;
    }

    // Debugging: Log the configuration
    std::cerr << "\nConfiguration for URI: " << request.getPath() << std::endl;
    std::map<std::string, std::vector<std::string> >::iterator it;
    for (it = config.begin(); it != config.end(); ++it)
    {
        std::cerr << it->first << ": ";
        std::vector<std::string>::iterator vecIt;
        for (vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt)
        {
            std::cerr << *vecIt << " ";
        }
        std::cerr << std::endl;
    }

    // Retrieve static directory and allowed paths from locInfo
    std::string staticDir = (config.find("root_directory") != config.end() && !config["root_directory"].empty())
                                ? config["root_directory"][0]
                                : "/public";
    std::vector<std::string> allowedPaths = config["allowed_paths"];
    std::vector<std::string> allowedMethods = config["allowed_methods"];
    std::string filePath = staticDir + request.getPath();

    // Set basic headers
    response.setHeader("Connection", "close");

    // Validate the HTTP method
    if (std::find(allowedMethods.begin(), allowedMethods.end(), request.getMethod()) == allowedMethods.end())
    {
        std::cerr << "HTTP method not allowed: " << request.getMethod() << std::endl;
        setErrorResponse(request, response, 405, "Method Not Allowed");
        return response;
    }

    // Validate the request path
    bool pathAllowed = false;
    std::vector<std::string>::iterator pathIt;
    for (pathIt = allowedPaths.begin(); pathIt != allowedPaths.end(); ++pathIt)
    {
        if (request.getPath() == *pathIt || request.getPath().find(*pathIt) == 0)
        {
            pathAllowed = true;
            break;
        }
    }

    if (!pathAllowed)
    {
        std::cerr << "Access denied for path: " << request.getPath() << std::endl;
        setErrorResponse(request, response, 403, "Forbidden");
        return response;
    }

    // Handle request methods
    if (request.getMethod() == "GET")
    {
        response = handleGet(request, response);
        return response;
    }
    else if (request.getMethod() == "POST")
    {
        std::cout << request << std::endl;
        request.handleFileUpload(request.getBody(), request.getPath(), response);
        return response;
    }
    else if (request.getMethod() == "DELETE")
    {
        std::string path = staticDir + request.getPath();
        path = urlDecode(path);

        if (remove(path.c_str()) == 0)
        {
            response.setStatusCode(200);
            response.setStatusMsg("OK");
            response.setBody("Resource deleted successfully.");
        }
        else
        {
            response.setStatusCode(404);
            response.setStatusMsg("Not Found");
            response.setBody("Resource not found.");
        }
        return response;
    }

    // If no supported method matched
    setErrorResponse(request, response, 405, "Method Not Supported");
    return response;
}
