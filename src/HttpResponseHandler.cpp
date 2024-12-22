#include "HttpResponseHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "RequestResponseState.hpp"

void HttpResponseHandler::handleResponse(RRState& rrstate)
{
    try
    {
        *this = handlePath(rrstate);
        std::string responseStr = getAll();
        size_t totalSent = 0;
        ssize_t sent;
        while (totalSent < responseStr.length())
        {
            sent = send(rrstate.getClientSock(), 
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