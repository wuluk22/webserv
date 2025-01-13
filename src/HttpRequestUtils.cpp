# include "HttpRequestHandler.hpp"
# include "RequestResponseState.hpp"

std::string HttpRequestHandler::extractDir(std::string& requestPath) {
    if (requestPath.empty()) {
        return "";
    }
    size_t pos = requestPath.find('/');
    if (pos == std::string::npos) {
        return "";
    }
    size_t dirStartPos = requestPath.find('/', pos + 1);
    if (dirStartPos == std::string::npos) {
        return "";
    }
    size_t dirEndPos = requestPath.find('/', dirStartPos + 1);
    if (dirEndPos != std::string::npos) {
        return requestPath.substr(dirStartPos, dirEndPos - dirStartPos);
    }
    return requestPath.substr(dirStartPos);
}

std::string HttpRequestHandler::removeExcessiveSlashes(std::string& path) {
	std::string result;
	bool was_last_slash = false;

	for (std::string::size_type i = 0; i < path.size(); ++i) {
		char c = path[i];

		if (c == '/') {
			if (!was_last_slash) {
				result += c;
				was_last_slash = true;
			}
		} else {
			result += c;
			was_last_slash = false;
		}
	}
    return (result);
}

LocationBlock* HttpRequestHandler::getLocationBlock(RRState& rrstate, std::vector<LocationBlock*> locationBlocks) const
{
    std::string requestPath = this->getPath();
    LocationBlock* matchedBlock = NULL;
    size_t longestMatch = 0;
    std::string requestSwap;
	
	// Check perfect match 
	std::vector<LocationBlock*>::const_iterator it = locationBlocks.begin();
    for (;it != locationBlocks.end(); ++it) {
        LocationBlock* block = *it;
        const std::string& locationUri = block->getLocationParams()._uri;
        if (requestPath == locationUri)
            return (block);
    }
	// Check closest-match
    it = locationBlocks.begin();
	for (;it != locationBlocks.end(); ++it) {
        LocationBlock* block = *it;
        const std::string& locationUri = block->getLocationParams()._uri;
        if (locationUri == "/" && requestPath.length() > locationUri.length() &&
            it != locationBlocks.end() && it == locationBlocks.begin()) {
            continue;
        }
        if (!block->isCgiAllowed())
            requestSwap = rrstate.getRequest().extractDir(requestPath);
        else
            requestSwap = locationUri;
        if (requestSwap == locationUri)
            requestPath = requestSwap;
        if (requestPath == locationUri) {
            size_t matchLength = locationUri.length();
            if (matchLength > longestMatch) 
            {
                longestMatch = matchLength;
                matchedBlock = block;
            }
        }
    }
    return (matchedBlock);
}


std::string HttpRequestHandler::trim(const std::string& str)
{
	size_t	first;
	size_t	last;

    first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
		return "";
    last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

bool HttpRequestHandler::endsWith(const std::string& str, const std::string& suffix)
{
    if (str.length() < suffix.length())
		return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool HttpRequestHandler::isMethodAllowed(const HttpRequestHandler& request, const std::string& method) const
{
	std::vector<std::string> alwdMet = request.getAllowedMethods();
	for (size_t i = 0; i < alwdMet.size(); i++)
	{
			if (alwdMet[i] == method)
			{
					return true;
			}
	}
	return false;
}

bool HttpRequestHandler::isPathAllowed(const HttpRequestHandler& request, const std::string& path) const
{
	std::vector<std::string> alwdPat = request.getAllowedPaths();
	for (size_t i = 0; i < alwdPat.size(); i++)
	{
			if (alwdPat[i] == path)
			{
					return true;
			}
	}
	return false;
}

std::string HttpRequestHandler::toString(size_t value)
{
    std::ostringstream	oss;
    oss << value;
    return oss.str();
}

std::string HttpRequestHandler::getMimeType(const std::string& path)
{
    if (endsWith(path, ".html")) return "text/html";
    if (endsWith(path, ".css")) return "text/css";
    if (endsWith(path, ".js")) return "application/javascript";
	if (endsWith(path, ".json")) return "application/json";
    if (endsWith(path, ".png")) return "image/png";
    if (endsWith(path, ".jpg") || endsWith(path, ".jpeg")) return "image/jpeg";
    if (endsWith(path, ".gif")) return "image/gif";
    return "text/plain";
}

std::string HttpRequestHandler::createErrorPage(int statusCode, const std::string& message)
{
    std::ostringstream	oss;
    oss << "<html><head><title>" << statusCode << " - " << message << "</title></head>";
    oss << "<body><h1>" << statusCode << " - " << message << "</h1></body></html>";
    return oss.str();
}

bool HttpRequestHandler::fileExists(const std::string& path)
{
    struct stat	buffer;
    return stat(path.c_str(), &buffer) == 0;
}

std::ostream& operator<<(std::ostream& out, const HttpRequestHandler& handler)
{
	out << "\n---------------------------REQUEST---------------------------------\n";
    out << handler.getMethod() << " " << handler.getPath() << " " << handler.getHttpVersion() << "\n";
    const std::map<std::string, std::string>&	headers = handler.getHeaders();
	const std::vector<std::string>& 			allowedMethods = handler.getAllowedMethods();
	const std::vector<std::string>&				allowedPaths = handler.getAllowedPaths();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
         it != headers.end(); ++it)
	{
        out << it->first << ": " << it->second << "\n";
    }
	out << "\nallowedMethods : \n";
	for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); ++it)
	{
		out << *it << "\n";
	}
	out << "\nallowedPaths : \n";
	for (std::vector<std::string>::const_iterator it = allowedPaths.begin(); it != allowedPaths.end(); ++it)
	{
		out << *it << "\n";
	}
	out << "\nallowedPath : \n";
	out << handler.getAllowedPath() << "\n";
	out << "\n \n";
	out << handler.getAutoIndex() << "\n";
	out << "\nmaxBody: \n";
	out << handler.getMaxBody() << "\n";
	out << "\n---------------------------REQUEST---------------------------------\n";
    return out;
}

std::string HttpRequestHandler::readFile(RRState& rrstate, const std::string& path)
{
	std::string newPath;
	std::string root;
    size_t pos;
    std::string imagesPathCgi = rrstate.getServer().getImagesPathCgi();

	root = rrstate.getRequest().getLocationBlock(rrstate, rrstate.getServer().getLocations())->getRoot();
    pos = imagesPathCgi.find(root);
    if (pos != std::string::npos) {
        imagesPathCgi.erase(pos, root.length());
    }
	if (path.compare(imagesPathCgi) >= 0) {
		newPath = root + path;
	}
	else
		newPath = path;
    std::ifstream		file(newPath.c_str(), std::ios::binary);
	
	size_t				size;
    if (!file) return "";
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
	std::vector<char>	buffer(size);
    file.read(&buffer[0], size);
    return std::string(buffer.begin(), buffer.end());
}

std::string HttpRequestHandler::extractBoundary(const std::string& contentType)
{
    size_t	pos;
	pos = contentType.find("boundary=");
    if (pos != std::string::npos)
	{
        return "--" + contentType.substr(pos + 9);
    }
    return "";
}

std::string HttpRequestHandler::generateErrorResponse(const std::string& message)
{
	std::ostringstream	response;
	response << "HTTP/1.1 400 Bad Request\r\n"
			<< "Content-Type: text/plain\r\n"
			<< "Content-Length: " << message.length() << "\r\n"
			<< "\r\n"
			<< message;
	return response.str();
}

void 																			HttpRequestHandler::setMethod(const std::string& m) { method = m; }
void 																			HttpRequestHandler::setPath(const std::string& p) { path = p; }
void 																			HttpRequestHandler::setHttpVersion(const std::string& h) { httpVersion = h; }
void 																			HttpRequestHandler::setHeader(const std::string& name, const std::string& value) { headers[trim(name)] = trim(value); }
void 																			HttpRequestHandler::setBody(const std::string& b) { body = b; }
void 																			HttpRequestHandler::setFd(const int& nb) { fd = nb; }
void 																			HttpRequestHandler::setIsComplete(const bool& is) { isRequestComplete = is; }
void 																			HttpRequestHandler::setAllowedMethods(const std::vector<std::string>& methods) { allowedMethods = methods; }
void 																			HttpRequestHandler::setAllowedPaths(const std::vector<std::string>& paths) { allowedPaths = paths; }
void 																			HttpRequestHandler::setAllowedPath(const std::string& path) { allowedPath = path; }
void 																			HttpRequestHandler::setRootDirectory(const std::string& path) { rootDirectory = path; }
void 																			HttpRequestHandler::setCgiPath(const std::vector<std::string>& cgiPath) { _CgiPath = cgiPath; }
void 																			HttpRequestHandler::setClientSocket(const int& clientSock) {_clientSocket = clientSock;}
																			
void 																			HttpRequestHandler::setLocInfo(const std::map<std::string, std::map<std::string, std::vector<std::string> > >& locInfo) { _locInfo = locInfo; }
void 																			HttpRequestHandler::setIsValid(const bool& val) { valid = val; }
void 																			HttpRequestHandler::setAutoIndex(const bool& index) { autoIndex = index; }
void 																			HttpRequestHandler::setMaxBody(const unsigned int& max) { maxBodySize = max; }

std::string 																	HttpRequestHandler::getMethod() const { return method; }
std::string 																	HttpRequestHandler::getPath() const { return path; }
std::string 																	HttpRequestHandler::getHttpVersion() const { return httpVersion; }
std::string 																	HttpRequestHandler::getBody() const { return body; }
std::string 																	HttpRequestHandler::getHeader(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(name);
    return it != headers.end() ? it->second : "";
}
const std::map<std::string, std::string>&										HttpRequestHandler::getHeaders() const { return headers; }
int     																		HttpRequestHandler::getFd() const { return fd; }
bool    																		HttpRequestHandler::getIsComplete() const { return isRequestComplete; }
std::string																		HttpRequestHandler::getRootDirectory() const { return rootDirectory; }
const std::vector<std::string>& 												HttpRequestHandler::getAllowedMethods() const { return allowedMethods; }
const std::vector<std::string>& 												HttpRequestHandler::getAllowedPaths() const { return allowedPaths; }
const std::string&																HttpRequestHandler::getAllowedPath() const { return allowedPath; }
const std::vector<std::string>&													HttpRequestHandler::getCgiPath() const { return _CgiPath;}
const int&																		HttpRequestHandler::getClientSocket() const {return _clientSocket; };
const std::map<std::string, std::map<std::string, std::vector<std::string> > >&	HttpRequestHandler::getLocInfo() const { return _locInfo; }
bool																			HttpRequestHandler::getIsValid() const { return valid; }
bool																			HttpRequestHandler::getAutoIndex() const { return autoIndex; }
const unsigned int&																HttpRequestHandler::getMaxBody() const { return maxBodySize; }