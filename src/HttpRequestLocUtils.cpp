#include "HttpRequestHandler.hpp"

std::map<std::string, std::vector<std::string> > HttpRequestHandler::getLocInfoByUri(HttpRequestHandler request)
{
    std::string requestUri = request.getPath();
    bool isRoot = (requestUri == "/");
    if (!isRoot && requestUri.find("/public") != 0)
    {
        requestUri = "/public" + requestUri;
    }

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

std::vector<std::string>    HttpRequestHandler::getContentPathsFromLoc(const std::string& uri) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find("content_path") != _locInfo.find(uri)->second.end())
    {
        return _locInfo.find(uri)->second.find("content_path")->second;
    }
    return std::vector<std::string>();
}

std::string HttpRequestHandler::getRootDirectoryFromLoc(const std::string& uri) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find("root_directory") != _locInfo.find(uri)->second.end())
    {
        const std::vector<std::string>& roots = _locInfo.find(uri)->second.find("root_directory")->second;
        if (!roots.empty())
        {
            return roots[0];
        }
    }
    return "";
}

bool HttpRequestHandler::isAutoIndexEnabled(const std::string& uri) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find("auto_index") != _locInfo.find(uri)->second.end())
    {
        const std::vector<std::string>& autoIndex = _locInfo.find(uri)->second.find("auto_index")->second;
        return !autoIndex.empty() && autoIndex[0] == "on";
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

std::vector<std::string> HttpRequestHandler::getConfigFieldFromLoc(const std::string& uri, const std::string& field) const
{
    if (_locInfo.find(uri) != _locInfo.end() && _locInfo.find(uri)->second.find(field) != _locInfo.find(uri)->second.end())
    {
        return _locInfo.find(uri)->second.find(field)->second;
    }
    return std::vector<std::string>();
}

std::string HttpRequestHandler::getFullPathFromLoc(const std::string& relativePath) const
{
    std::string rootDirectory = getRootDirectoryFromLoc(relativePath);
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

bool HttpRequestHandler::isAutoIndexEnabledForUri(const std::string& uri) const
{
    return isAutoIndexEnabled(uri);
}

bool HttpRequestHandler::isPathAllowedInLoc(const std::string& uri, const std::string& path) const
{
    std::vector<std::string> contentPaths = getContentPathsFromLoc(uri);
    for (std::vector<std::string>::const_iterator it = contentPaths.begin(); it != contentPaths.end(); ++it)
    {
        if (*it == path)
        {
            return true;
        }
    }
    return false;
}
