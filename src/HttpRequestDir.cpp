#include "HttpRequestHandler.hpp"
#include "HttpResponseHandler.hpp"

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
    std::cout << response << std::endl;
}

void HttpRequestHandler::handleFileUpload(const std::string& requestData, const std::string& path, HttpResponseHandler& response)
{
    std::string				contentType;
    std::string				boundary;
	std::string				filename;
	std::string				fileContent;
	std::string				uploadPath;
	std::string				fullPath;
	std::string::size_type	pos;
	std::string::size_type	endPos;
	std::string::size_type	contentEnd;

	contentType = getHeader("Content-Type");
    /*
    BODY SIZE ------------------------------------------
    std::string contentSize = getHeader("Content-Size");
    std::cout << "\n\nKOOOOO : " << contentSize << std::endl;*/
	boundary = extractBoundary(contentType); 
    if (boundary.empty())
    {
        throw std::runtime_error("Missing or invalid boundary in multipart form data");
    }

    // Find the boundary in the request data
    pos = requestData.find(boundary);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Invalid multipart form data");
    }

    // Find the filename
    pos = requestData.find("filename=\"", pos);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("No filename found in upload");
    }

    pos += 10; // length of 'filename="'
    endPos = requestData.find("\"", pos);
    if (endPos == std::string::npos)
    {
        throw std::runtime_error("Invalid filename format");
    }

    filename = requestData.substr(pos, endPos - pos);
    
    // Find the start of file content (after the headers)
    pos = requestData.find("\r\n\r\n", endPos);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Invalid file content format");
    }
    pos += 4;

    // Find the end of the file content (before the closing boundary)
    contentEnd = requestData.find(boundary, pos);
    if (contentEnd == std::string::npos)
    {
        throw std::runtime_error("Invalid file content ending");
    }
    contentEnd -= 4;  // Account for the \r\n-- before the boundary

    // Extract file content
    fileContent = requestData.substr(pos, contentEnd - pos);

    // Ensure directory exists
    uploadPath = "public" + path;
    if (!DirectoryHandler::isDirectory(uploadPath.c_str()))
    {
        if (!DirectoryHandler::createDirectory(uploadPath.c_str()))
        {
            throw std::runtime_error("Failed to create upload directory");
        }
    }

    // Create file path
    fullPath = uploadPath + "/" + filename;
    
    // Save file
	std::ofstream	file(fullPath.c_str(), std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to create output file");
    }

    file.write(fileContent.c_str(), fileContent.length());
    file.close();

    // Redirect back to directory listing
    response.setHttpVersion("HTTP/1.1");
    response.setStatusCode(302);
    response.setStatusMsg("Found");
    response.setHeader("Location", path);
    response.setHeader("Content-Length", "0");
}
