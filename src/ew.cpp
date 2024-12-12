std::map<std::string, std::vector<std::string> > HttpRequestHandler::getLocInfoByUri(HttpRequestHandler request)
{
    const std::string& requestUri = request.getPath();
    
    // Iterate through the location info map
    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;
        const std::map<std::string, std::vector<std::string> >& locConfig = it->second;
        
        // Check if the URI matches the location
        if (locationUri == "/")
        {
            std::cout << "Checking location: " << locationUri << " against request: " << requestUri << std::endl;
            
            // Check if the request URI is the same as the location
            if (requestUri == locationUri || requestUri == locationUri + "/")
            {
                // If the URI matches the location, check for index files
                std::map<std::string, std::vector<std::string> >::const_iterator indexIt = locConfig.find("index");
                if (indexIt != locConfig.end())
                {
                    // If index files are defined, check if the request URI matches one of them
                    const std::vector<std::string>& indexFiles = indexIt->second;
                    for (std::vector<std::string>::const_iterator fileIt = indexFiles.begin(); fileIt != indexFiles.end(); ++fileIt)
                    {
                        // Resolve the path to the index file (e.g., /public/docs.html)
                        std::string indexFilePath = locConfig.at("root_directory")[0] + *fileIt;  // assuming root_directory exists
                        if (requestUri == *fileIt || requestUri == indexFilePath)
                        {
                            std::cout << "MATCH: Found index file " << *fileIt << " for URI: " << requestUri << std::endl;
                            this->setIsValid(true);  // Mark the request as valid
                            return locConfig;  // Return the location's config
                        }
                    }
                }
            }
        }
    }

    // No match found
    this->setIsValid(false);
    return std::map<std::string, std::vector<std::string> >();  // Return an empty map if no match
}
