#include "HttpResponseHandler.hpp"

std::string urlDecode(const std::string& url)
{
    std::string decoded;
    size_t length = url.length();
    
    for (size_t i = 0; i < length; ++i)
	{
        if (url[i] == '%')
		{
            if (i + 2 < length)
			{
                std::string hex_value = url.substr(i + 1, 2);
                char decoded_char = static_cast<char>(strtol(hex_value.c_str(), NULL, 16));
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

bool isCgiRequest(const std::string& path)
{
    const char* cgiExtensionsArray[] = {".cgi", ".pl", ".py"};
    std::vector<std::string> cgiExtensions(cgiExtensionsArray, cgiExtensionsArray + sizeof(cgiExtensionsArray) / sizeof(cgiExtensionsArray[0]));

    const std::string cgiDirectory = "/cgi-bin";
	std::cout << "\nPATH: " << path << std::endl;

    if (path.find(cgiDirectory) == 0)
	{
        return true;
    }

    /*for (std::vector<std::string>::const_iterator it = cgiExtensions.begin(); it != cgiExtensions.end(); ++it) {
        const std::string& ext = *it;
        if (path.length() >= ext.length() &&
            path.compare(path.length() - ext.length(), ext.length(), ext) == 0) {
            struct stat fileStat;
            std::string fullPath = "cgi-bin" + path.substr(cgiDirectory.length());
            if (stat(fullPath.c_str(), &fileStat) == 0 && (fileStat.st_mode & S_IXUSR)) {
                return true;
            }
        }
    }*/
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
        // Log failure in sending error response
        std::cerr << "Error sending error response: " << e.what() << std::endl;
    }
}

std::ostream	&operator<<(std::ostream &out, const HttpResponseHandler &i)
{
	out << i.getHttpVersion() << " " << i.getStatusCode() << " " << i.getStatusMsg() << std::endl;
	std::map<std::string, std::string> headers = i.getHeaders();
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		out << it->first << ": " << it->second << std::endl;
	}
	out << i.getBody();
	return out;
}

void	HttpResponseHandler::setHttpVersion(std::string version) { this->httpVersion = version; }
void	HttpResponseHandler::setStatusCode(int code) { this->code = code; }
void	HttpResponseHandler::setStatusMsg(std::string message) { this->status = message; }
void	HttpResponseHandler::setHeader(const std::string &headerName, const std::string &headerValue) { headers[headerName] = headerValue; }
void	HttpResponseHandler::setBody(std::string body) { this->body = body; }

std::string HttpResponseHandler::getHttpVersion() const { return httpVersion; }
int         HttpResponseHandler::getStatusCode() const { return code; }
std::string HttpResponseHandler::getStatusMsg() const { return status; }
std::string HttpResponseHandler::getHeader(const std::string &headerName) const
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

