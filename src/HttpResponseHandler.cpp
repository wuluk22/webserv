#include "HttpResponseHandler.hpp"

int HttpResponseHandler::handle_response(int client_sock)
{
    // Simple HTTP Response
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n\r\n"
        "Hello, World!";

    send(client_sock, response.c_str(), response.length(), 0);
    std::cout << "[Response sent to client]" << std::endl;
	return 0;
}