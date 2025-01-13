#include "HttpResponseHandler.hpp"
#include "RequestResponseState.hpp"

std::string HttpResponseHandler::getPathOfFile(RRState& rrstate)
{
	std::string	filePath;
	std::string uri;
	std::string root;
	
	root = rrstate.getRequest().getLocationBlock(rrstate, rrstate.getServer().getLocations())->getRoot();
    uri = rrstate.getRequest().getPath();
	filePath = root + uri;
	struct stat info;
	if (stat(filePath.c_str(), &info) == 0 && S_ISDIR(info.st_mode))
	{ 
		return "";
	}
	else if (stat(filePath.c_str(), &info) == 0 && S_ISREG(info.st_mode))
	{
		return filePath;
	}
	return filePath;
}

std::string HttpResponseHandler::urlDecode(const std::string& url)
{
    std::string decoded;
    size_t      length = url.length();
    for (size_t i = 0; i < length; ++i)
	{
        if (url[i] == '%')
		{
            if (i + 2 < length)
			{
                std::string hex_value = url.substr(i + 1, 2);
                char        decoded_char = static_cast<char>(strtol(hex_value.c_str(), NULL, 16));
                decoded += decoded_char;
                i += 2;
            }
        }
		else if (url[i] == '+')
		{
            decoded += ' ';
        }
		else
		{
            decoded += url[i];
        }
    }

    return decoded;
}

bool HttpResponseHandler::isCgiRequest(RRState& rrstate, const std::string& path)
{	
	std::string cgiDir;
	std::vector<LocationBlock *> loc = rrstate.getServer().getLocations();

	for (std::vector<LocationBlock *>::iterator it = loc.begin(); it != loc.end(); it++) {
		if (!((*it)->getUriDependance().empty())) {
			cgiDir = (*it)->getUri();
			break;
		}
	}
	if (!cgiDir.empty() && path.find(cgiDir) == 0) {
        return true;
    }
    return false;
}

std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i)
{
	out << "\n---------------------------RESPONSE---------------------------------\n";
	out << i.getHttpVersion() << " " << i.getStatusCode() << " " << i.getStatusMsg() << std::endl;
	std::map<std::string, std::string> headers = i.getHeaders();
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		out << it->first << ": " << it->second << std::endl;
	}
	out << i.getBody();
	out << "\n---------------------------RESPONSE---------------------------------\n";
	return out;
}

void	                            HttpResponseHandler::setHttpVersion(std::string version) { this->httpVersion = version; }
void	                            HttpResponseHandler::setStatusCode(int code) { this->code = code; }
void	                            HttpResponseHandler::setStatusMsg(std::string message) { this->status = message; }
void	                            HttpResponseHandler::setHeader(const std::string &headerName, const std::string &headerValue) { headers[headerName] = headerValue; }
void	                            HttpResponseHandler::setBody(std::string body) { this->body = body; }
void                                HttpResponseHandler::setQuery(std::string query) { this->_query = query;}
void	                            HttpResponseHandler::setResponse(std::string output) {}
std::string                         HttpResponseHandler::getHttpVersion() const { return httpVersion; }
int                                 HttpResponseHandler::getStatusCode() const { return code; }
std::string                         HttpResponseHandler::getStatusMsg() const { return status; }
std::string                         HttpResponseHandler::getHeader(const std::string &headerName) const
{
	std::map<std::string, std::string>::const_iterator it = headers.find(headerName);
	if (it != headers.end())
	{
		return it->second;
	}
	return "";
}
std::map<std::string, std::string>  HttpResponseHandler::getHeaders() const { return headers; }
std::string                         HttpResponseHandler::getBody() const { return body; }
std::string                         HttpResponseHandler::getAll() const
{
	std::ostringstream all;
	all << httpVersion << " " << code << " " << status << "\r\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		all << it->first << ": " << it->second << "\r\n";
	}
	all << "\r\n";
	all << body;
	return all.str();
}
std::string							HttpResponseHandler::getQuery() const { return _query;}