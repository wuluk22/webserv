#include "HttpRequestHandler.hpp"

std::map<std::string, std::vector<std::string> > HttpRequestHandler::getLocInfoByUri(HttpRequestHandler request)
{
    const std::string& requestUri = request.getPath();

    
    for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = _locInfo.begin(); it != _locInfo.end(); ++it)
    {
        const std::string& locationUri = it->first;
        const std::map<std::string, std::vector<std::string> >& locConfig = it->second;
        
        //const std::string& locationUri = it->first; // remplacer it->first par std::vector<std::string> paths;
        /*
            pour /docs.html je dois pouvoir savoir qu'il se trouve dans le it->first : "/public"
            pour se faire je dois ajouter public a /docs.html pour pouvoir le comparer a it->first
            ou directement aux elements de it->second["index"]

            structure de locInfo:
            
            it->first       it->second              elements of it->second
            "/"             ["allowed_methods"      ["GET", "POST"]]
                            ["content_path"         ["/fullfilepath/"]]
                            ["root_directory"       ["public"]]
                            ["index"                ["/public/index.html, /public/docs.html"]]
        */
        std::cout << "-----------locationUri: " << locationUri << "   ------------requestUri: " << requestUri << std::endl;
        if (requestUri == locationUri)
        {
			std::cout << "MATCH between: " << locationUri << " and " << requestUri << std::endl;
			this->setIsValid(true);
            std::cout << "--------------------isvalid : " << request.getIsValid() << std::endl;
            return it->second;
        }
    }
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

std::string HttpRequestHandler::getFullPathFromLoc(const std::string& uri, const std::string& relativePath) const
{
    std::string rootDirectory = getRootDirectoryFromLoc(uri);
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
