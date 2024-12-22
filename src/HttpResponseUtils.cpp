#include "HttpResponseHandler.hpp"

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

bool HttpResponseHandler::isCgiRequest(const std::string& path)
{
    const char* cgiExtensionsArray[] = {".cgi", ".pl", ".py"};
    std::vector<std::string> cgiExtensions(cgiExtensionsArray, cgiExtensionsArray + sizeof(cgiExtensionsArray) / sizeof(cgiExtensionsArray[0]));
    const std::string cgiDirectory = "/cgi-bin";
    if (path.find(cgiDirectory) == 0)
	{
        return true;
    }
    return false;
}

void HttpResponseHandler::sendError(int clientSock, int statusCode, const std::string& statusMsg, const std::string& body)
{
    try
    {
        setHttpVersion("HTTP/1.1");
        setStatusCode(statusCode);
        setStatusMsg(statusMsg);
        setHeader("Content-Type", "text/plain");
        std::ostringstream oss;
        oss << body.length();
        setHeader("Content-Length", oss.str());
        setBody(body);
        std::string responseStr = getAll();
        send(clientSock, responseStr.c_str(), responseStr.length(), 0);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error sending error response: " << e.what() << std::endl;
    }
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