#include "HttpRequestHandler.hpp"




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

bool HttpRequestHandler::isMethodAllowed(HttpRequestHandler request, const std::string& method) const
{
    // Retrieve allowed methods from locInfo
    std::map<std::string, std::vector<std::string> > locInfo = request.getLocInfoByUri(request);

    // Check if the "allowed_methods" key exists and contains the requested method
    if (locInfo.find("allowed_methods") != locInfo.end())
    {
        const std::vector<std::string>& allowedMethods = locInfo.at("allowed_methods");
        for (size_t i = 0; i < allowedMethods.size(); i++)
        {
            if (allowedMethods[i] == method)
            {
                return true;
            }
        }
    }

    // If no match is found, return false
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
	out << "\n---------------------------REQUEST---------------------------------\n";
    return out;
}

std::string HttpRequestHandler::readFile(const std::string& path)
{
    std::ifstream		file(path.c_str(), std::ios::binary);
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

void HttpRequestHandler::setMethod(const std::string& m) { method = m; }
void HttpRequestHandler::setPath(const std::string& p) { path = p; }
void HttpRequestHandler::setHttpVersion(const std::string& h) { httpVersion = h; }
void HttpRequestHandler::setHeader(const std::string& name, const std::string& value) { headers[trim(name)] = trim(value); }
void HttpRequestHandler::setBody(const std::string& b) { body = b; }
void HttpRequestHandler::setFd(const int& nb) { fd = nb; }
void HttpRequestHandler::setIsComplete(const bool& is) { isRequestComplete = is; }
void HttpRequestHandler::setAllowedMethods(const std::vector<std::string>& methods) { allowedMethods = methods; }
void HttpRequestHandler::setAllowedPaths(const std::vector<std::string>& paths) { allowedPaths = paths; }
void HttpRequestHandler::setAllowedPath(const std::string& path) { allowedPath = path; }
void HttpRequestHandler::setRootDirectory(const std::string& path) { rootDirectory = path; }
void HttpRequestHandler::setLocInfo(const std::map<std::string, std::map<std::string, std::vector<std::string> > >& locInfo) { _locInfo = locInfo; }
void HttpRequestHandler::setIsValid(const bool& val) { valid = val; }
//void HttpRequestHandler::setCgiPath(const std::vector<std::string>& cgiPath) { _CgiPath = cgiPath; }

std::string HttpRequestHandler::getMethod() const { return method; }
std::string HttpRequestHandler::getPath() const { return path; }
std::string HttpRequestHandler::getHttpVersion() const { return httpVersion; }
std::string HttpRequestHandler::getBody() const { return body; }
std::string HttpRequestHandler::getHeader(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = headers.find(name);
    return it != headers.end() ? it->second : "";
}
const   std::map<std::string, std::string>& HttpRequestHandler::getHeaders() const { return headers; }
int     HttpRequestHandler::getFd() const { return fd; }
bool    HttpRequestHandler::getIsComplete() const { return isRequestComplete; }
std::string	HttpRequestHandler::getRootDirectory() const { return rootDirectory; }
const std::vector<std::string>& HttpRequestHandler::getAllowedMethods() const { return allowedMethods; }
const std::vector<std::string>& HttpRequestHandler::getAllowedPaths() const { return allowedPaths; }
const std::string&				HttpRequestHandler::getAllowedPath() const { return allowedPath; }
const std::map<std::string, std::map<std::string, std::vector<std::string> > >&	HttpRequestHandler::getLocInfo() const { return _locInfo; }
bool	HttpRequestHandler::getIsValid() const { return valid; }
//const std::vector<std::string>& HttpRequestHandler::getCgiPath() const { return _CgiPath;}
