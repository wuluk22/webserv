#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1024;
const int PORT = 8080;

void set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
	{
        std::cerr << "Error in fcntl: unable to get flags" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
        std::cerr << "Error in fcntl: unable to set non-blocking" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int server_fd, new_socket;
    int client_sockets[MAX_CLIENTS];
    int activity, max_sd, sd;
    int addrlen;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // Initialize all client sockets to 0 (not checked in use)
    for (int i = 0; i < MAX_CLIENTS; i++)
	{
        client_sockets[i] = 0;
    }

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
	{
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server socket created with fd: " << server_fd << std::endl;

    // Set server socket to reuse address (prevents "Bind failed" error on restart)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
	{
        std::cerr << "setsockopt failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking mode
    set_nonblocking(server_fd);

    // Setup server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    addrlen = sizeof(address);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server socket bound to port " << PORT << std::endl;

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
	{
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server is listening for connections" << std::endl;

    while (true)
	{
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add server socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++)
		{
            sd = client_sockets[i];
            if (sd > 0)
			{
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
			{
                max_sd = sd;
            }
        }

        // Wait for an activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
		{
            std::cerr << "Select error" << std::endl;
        }

        // If something happened on the server socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds))
		{
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0)
			{
                std::cerr << "Accept failed" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "DEBUG: New connection, socket fd: " << new_socket
                      << ", IP: " << inet_ntoa(address.sin_addr)
                      << ", Port: " << ntohs(address.sin_port) << std::endl;

            // Add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++)
			{
                if (client_sockets[i] == 0)
				{
                    client_sockets[i] = new_socket;
                    std::cout << "DEBUG: Adding to list of sockets as " << i << std::endl;
                    break;
                }
            }
        }

        // Handle I/O operation on client sockets
        for (int i = 0; i < MAX_CLIENTS; i++)
		{
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds))
			{
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0)
				{
                    // Client disconnected
                    std::cout << "DEBUG: Client disconnected, socket fd: " << sd << std::endl;
                    close(sd);
                    client_sockets[i] = 0;
                }
				else
				{
                    buffer[valread] = '\0';
                    std::cout << "DEBUG: Received data from client " << i << ": " << buffer << std::endl;

                    // Simple HTTP response
                    const char *response = 
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 13\r\n"
                        "Connection: close\r\n\r\n"
                        "Hello, World!";
                    
                    // Send the response
                    write(sd, response, strlen(response));
                    std::cout << "DEBUG: Sent response to client " << i << std::endl;

                    // Close the connection
                    close(sd);
                    client_sockets[i] = 0;
                }
            }
        }
    }
    return 0;
}
