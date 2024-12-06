#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"

void HttpResponseHandler::handleResponse(HttpRequestHandler& request, int clientSock)
{
    try
    {
        *this = handlePath(request, *this);

        std::string responseStr = getAll();
        size_t totalSent = 0;
        ssize_t sent;

        while (totalSent < responseStr.length())
        {
            sent = send(clientSock, 
                        responseStr.c_str() + totalSent, 
                        responseStr.length() - totalSent, 
                        0);

            if (sent <= 0)
                throw std::runtime_error("Failed to send response to client.");
            
            totalSent += sent;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error sending response: " << e.what() << std::endl;
        throw;
    }
}

