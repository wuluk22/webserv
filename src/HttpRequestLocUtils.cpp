#include "HttpRequestHandler.hpp"
#include "RequestResponseState.hpp"

std::map<std::string, std::vector<std::string> > HttpRequestHandler::getLocInfoByUri(HttpRequestHandler request)
{
    std::string requestUri = request.getPath();
    bool isRoot = (requestUri == "/");
    /*if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }*/
    /*std::cout << "\n\n---GET PATH: " << request.getPath() << std::endl;
    std::cout << "---GET C PATH: " << request.getRootDirectory() << std::endl;
    std::cout << "---GET A PATH: " << request.getAllowedPath() << std::endl;*/



    std::cout << "Adjusted REQUEST PATH: " << requestUri << std::endl;
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator matchedLocation = _locInfo.end();

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;

        if (requestUri.find(locationUri) == 0 && 
            (matchedLocation == _locInfo.end() || locationUri.size() > matchedLocation->first.size()))
        {
            matchedLocation = it;
        }
    }

    if (matchedLocation != _locInfo.end())
    {
        const std::map<std::string, std::vector<std::string> >& locConfig = matchedLocation->second;
        std::cout << "MATCH FOUND: Location URI: " << matchedLocation->first << " for Request URI: " << requestUri << std::endl;
        this->setIsValid(true);
        return locConfig;
    }

    std::cout << "No matching location found for Request URI: " << requestUri << std::endl;
    this->setIsValid(false);
    return std::map<std::string, std::vector<std::string> >();
}


std::vector<std::string>    HttpRequestHandler::getAllowedMethodsFromLoc(const std::string& uri) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find("allowed_methods") != _locInfo.find(uri)->second.end())
    {
        return _locInfo.find(uri)->second.find("allowed_methods")->second;
    }
    return std::vector<std::string>();
}

std::vector<std::string> HttpRequestHandler::getContentPathsFromLoc(RRState& rrstate, const std::string& uri) const
{
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator matchedLocation = _locInfo.end();

    std::string requestUri = uri;
    /*bool isRoot = (requestUri == "/");
    if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }*/

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;

        if (requestUri.find(locationUri) == 0 && 
            (matchedLocation == _locInfo.end() || locationUri.size() > matchedLocation->first.size()))
        {
            matchedLocation = it;
        }
    }

    if (matchedLocation == _locInfo.end())
        return std::vector<std::string>();

    const std::map<std::string, std::vector<std::string> >& locConfig = matchedLocation->second;
    std::map<std::string, std::vector<std::string> >::const_iterator contentPathIt = locConfig.find("content_path");
    if (contentPathIt != locConfig.end())
    {
        return contentPathIt->second;
    }
    return std::vector<std::string>();
}


std::string HttpRequestHandler::getRootDirectoryFromLoc(RRState& rrstate, const std::string& uri) const
{
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator matchedLocation = _locInfo.end();
    
    std::string requestUri = uri;
    /*bool isRoot = (requestUri == "/");
    //std::cout << "ROOT ??? : " << rrstate.getRequest().getRootDirectory() << std::endl;
    
    if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }*/

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;
        
        if (requestUri.find(locationUri) == 0 && 
            (matchedLocation == _locInfo.end() || locationUri.size() > matchedLocation->first.size()))
        {
            matchedLocation = it;
        }
    }

    if (matchedLocation != _locInfo.end())
    {
        const std::map<std::string, std::vector<std::string> >& locConfig = matchedLocation->second;
        std::map<std::string, std::vector<std::string> >::const_iterator rootDirIt = locConfig.find("root_directory");

        if (rootDirIt != locConfig.end() && !rootDirIt->second.empty())
        {
            return rootDirIt->second[0];
        }
    }
    return "";
}


bool HttpRequestHandler::isAutoIndexEnabled(RRState& rrstate, const std::string& uri) const
{
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator matchedLocation = _locInfo.end();

    std::string requestUri = uri;
    /*bool isRoot = (requestUri == "/");
    
    if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }*/

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;

        if (requestUri.find(locationUri) == 0 && 
            (matchedLocation == _locInfo.end() || locationUri.size() > matchedLocation->first.size()))
        {
            matchedLocation = it;
        }
    }

    const std::map<std::string, std::vector<std::string> >& locConfig = matchedLocation->second;
    std::map<std::string, std::vector<std::string> >::const_iterator autoIndexIt = locConfig.find("auto_index");

    if (autoIndexIt != locConfig.end() && !autoIndexIt->second.empty())
    {
        if (autoIndexIt->second[0] == "on")
        {
            return true;
        }
    }

    return false;
}

std::vector<std::string> HttpRequestHandler::getIndexFilesFromLoc(const std::string& uri) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find("index") != _locInfo.find(uri)->second.end())
    {
        return _locInfo.find(uri)->second.find("index")->second;
    }
    return std::vector<std::string>();
}

unsigned int HttpRequestHandler::getMaxBodyFromLoc(RRState& rrstate, const std::string& uri) const
{
    std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator matchedLocation = _locInfo.end();

    std::string requestUri = uri;
    /*bool isRoot = (requestUri == "/");

    if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }*/

    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;

        if (requestUri.find(locationUri) == 0 && 
            (matchedLocation == _locInfo.end() || locationUri.size() > matchedLocation->first.size()))
        {
            matchedLocation = it;
        }
    }

    if (matchedLocation == _locInfo.end())
    {
        return 1;
    }

    const std::map<std::string, std::vector<std::string> >& locConfig = matchedLocation->second;
    std::map<std::string, std::vector<std::string> >::const_iterator maxBodyIt = locConfig.find("max_body");

    if (maxBodyIt != locConfig.end() && !maxBodyIt->second.empty())
    {
        unsigned int maxBody = static_cast<unsigned int>(std::atoi(maxBodyIt->second[0].c_str()));
        return maxBody;
    }
    return 1;
}



std::vector<std::string> HttpRequestHandler::getConfigFieldFromLoc(const std::string& uri, const std::string& field) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find(field) != _locInfo.find(uri)->second.end())
    {
        return _locInfo.find(uri)->second.find(field)->second;
    }
    return std::vector<std::string>();
}

std::string HttpRequestHandler::getFullPathFromLoc(RRState& rrstate, const std::string& relativePath) const
{
    std::string rootDirectory = getRootDirectoryFromLoc(rrstate, relativePath);
    if (!rootDirectory.empty())
    {
        return rootDirectory + relativePath;
    }
    return "";
}

bool HttpRequestHandler::isMethodAllowedInLoc(const std::string& uri, const std::string& method) const
{
    std::vector<std::string> allowedMethods = getAllowedMethodsFromLoc(uri);
    for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); ++it)
    {
        if (*it == method)
        {
            return true;
        }
    }
    return false;
}

bool HttpRequestHandler::isIndexFile(const std::string& uri, const std::string& fileName) const
{
    std::vector<std::string> indexFiles = getIndexFilesFromLoc(uri);
    for (std::vector<std::string>::const_iterator it = indexFiles.begin(); it != indexFiles.end(); ++it)
    {
        if (*it == fileName)
        {
            return true;
        }
    }
    return false;
}

bool HttpRequestHandler::isAutoIndexEnabledForUri(RRState& rrstate, const std::string& uri) const
{
    return isAutoIndexEnabled(rrstate, uri);
}

bool HttpRequestHandler::isPathAllowedInLoc(RRState& rrstate, const std::string& uri, const std::string& path) const
{
    std::vector<std::string> contentPaths = getContentPathsFromLoc(rrstate, uri);
    for (std::vector<std::string>::const_iterator it = contentPaths.begin(); it != contentPaths.end(); ++it)
    {
        if (*it == path)
        {
            return true;
        }
    }
    return false;
}

std::string HttpRequestHandler::getContentPath(const std::map<std::string, std::vector<std::string> >& config)
{
    std::map<std::string, std::vector<std::string> >::const_iterator it = config.find("content_path");
    if (it != config.end() && !it->second.empty())
    {
        return it->second[0]; // Return the first string in the vector
    }
    return ""; // Return an empty string if not found or the vector is empty
}
std::string HttpRequestHandler::getContPath() { return contentPath; };
void HttpRequestHandler::setContentPath(const std::string& path) { this->contentPath = path; };
