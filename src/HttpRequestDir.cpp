#include "HttpRequestHandler.hpp"
#include "HttpResponseHandler.hpp"
#include "RequestResponseState.hpp"

void	HttpRequestHandler::handleDirectoryRequest(const std::string& path, HttpResponseHandler& response)
{
	std::string				dirPath;
	std::string				content;
	std::vector<FileInfo>	files;

	dirPath	= "public" + path;
	files = DirectoryHandler::getDirectoryListing(dirPath);
	content = DirectoryHandler::generateDirectoryPage(path, files);
	response.setHttpVersion("HTTP/1.1");
	response.setStatusCode(200);
	response.setStatusMsg("OK");
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", toString(content.length()));
	response.setBody(content);
    //std::cout << response << std::endl;
}

void HttpRequestHandler::handleFileUpload(RRState& rrstate, const std::string& requestData, const std::string& path, HttpResponseHandler& response)
{
    std::string contentType = getHeader("Content-Type");
    std::string filename;
    std::string fileContent = requestData;
    std::string uploadPath;
    std::string fullPath;

    unsigned int max = rrstate.getRequest().getMaxBodyFromLoc(rrstate, rrstate.getRequest().getPath());
    if (requestData.length() > max)
    {
        setErrorResponse(rrstate, 413, "Payload Too Large");
        return;
    }
    if (contentType == "plain/text") {
        time_t now = time(0);
        std::ostringstream oss;
        oss << "upload_" << now << ".txt";
        filename = oss.str();
    } else if (contentType.find("multipart/form-data") != std::string::npos) {
        std::string boundary = extractBoundary(contentType);
        if (boundary.empty()) {
            throw std::runtime_error("Missing or invalid boundary in multipart form data");
        }

        std::string::size_type pos = requestData.find(boundary);
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid multipart form data");
        }

        pos = requestData.find("filename=\"", pos);
        if (pos == std::string::npos) {
            throw std::runtime_error("No filename found in upload");
        }

        pos += 10;
        std::string::size_type endPos = requestData.find("\"", pos);
        if (endPos == std::string::npos) {
            throw std::runtime_error("Invalid filename format");
        }

        filename = requestData.substr(pos, endPos - pos);
        pos = requestData.find("\r\n\r\n", endPos);
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid file content format");
        }

        pos += 4;
        std::string::size_type contentEnd = requestData.find(boundary, pos);
        if (contentEnd == std::string::npos) {
            throw std::runtime_error("Invalid file content ending");
        }

        contentEnd -= 4;
        fileContent = requestData.substr(pos, contentEnd - pos);
    } else {
        throw std::runtime_error("Unsupported Content-Type: " + contentType);
    }

    uploadPath = "public" + path;
    if (!DirectoryHandler::isDirectory(uploadPath.c_str())) {
        if (!DirectoryHandler::createDirectory(uploadPath.c_str())) {
            throw std::runtime_error("Failed to create upload directory");
        }
    }

    fullPath = uploadPath + "/" + filename;
    std::ofstream file(fullPath.c_str(), std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create output file");
    }

    file.write(fileContent.c_str(), fileContent.length());
    file.close();

    response.setHttpVersion("HTTP/1.1");
    response.setStatusCode(201);
    response.setStatusMsg("Created");
    response.setHeader("Content-Type", "text/plain");
    response.setHeader("Content-Length", "0");
}
