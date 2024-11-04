#include "SocketHandler.hpp"
#include "HttpRequestHandler.hpp"
#include "Logger.hpp"

const int PORT = 8080;
const int MAX_CLIENTS = 10;

int main()
{
    int server_fd, new_socket, client_sockets[MAX_CLIENTS], sd, activity, max_sd;
    struct sockaddr_in address;
	socklen_t	addrlen;
    fd_set readfds;

    // Initialize all client sockets to 0 (not in use)
    memset(client_sockets, 0, sizeof(client_sockets));

    // Create server socket
    server_fd = SocketHandler::create_socket();

    // Set server socket to reuse address
    SocketHandler::set_socket_options(server_fd);

    // Bind the server socket to a port
    SocketHandler::bind_socket(server_fd, address, PORT);

    // Set server socket to non-blocking
    SocketHandler::set_nonblocking(server_fd);

    // Start listening for connections
    SocketHandler::listen_socket(server_fd, 3);
    Logger::log("Server listening for connections");

    addrlen = sizeof(address);

    while (true)
    {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add server socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (sd > 0)
                FD_SET(sd, &readfds);
            if (sd > max_sd)
                max_sd = sd;
        }

        // Wait for an activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            Logger::log_error("Select error");
        }

        // If activity on server socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds))
        {
            new_socket = SocketHandler::accept_connection(server_fd, address, addrlen);
            Logger::log("New connection accepted");

            // Add new socket to array of clients
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == 0)
                {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Handle IO operations on each client socket
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds))
            {
                HttpRequestHandler::handle_request(sd);
                client_sockets[i] = 0;  // Mark as available
            }
        }
    }

    return 0;
}

