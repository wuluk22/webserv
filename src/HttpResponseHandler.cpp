#include "HttpResponseHandler.hpp"

void HttpResponseHandler::handleResponse(HttpRequestHandler& request, int client_sock)
{
    try
    {
        // Generate the appropriate response for the request
        *this = handlePath(request, *this);

        // Send the response to the client
        std::string responseStr = getAll();
        size_t totalSent = 0;
        ssize_t sent;

        while (totalSent < responseStr.length())
        {
            sent = send(client_sock, 
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
        // If sending fails, log the error and rethrow to the caller
        std::cerr << "Error sending response: " << e.what() << std::endl;
        throw;
    }
}

