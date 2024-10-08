#include "HttpRequestHandler.hpp"

const int BUFFER_SIZE = 1024;

void HttpRequestHandler::handle_request(int client_sock)
{
    char buffer[BUFFER_SIZE];
    int valread = recv(client_sock, buffer, BUFFER_SIZE, 0);

    if (valread == 0)  // Client disconnected
    {
        std::cout << "Client disconnected, closing socket" << std::endl;
        close(client_sock);
        return;
    }
    else if (valread < 0)
    {
        std::cerr << "Error in receiving data" << std::endl;
        return;
    }

    buffer[valread] = '\0';
    std::cout << "Received request: " << buffer << std::endl;

    // Simple HTTP Response
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n\r\n"
        "Hello, World!";

    send(client_sock, response.c_str(), response.length(), 0);
    std::cout << "Response sent to client" << std::endl;

    close(client_sock);  // Close the client socket after sending response
}

