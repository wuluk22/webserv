#include <iostream>  // std::cin | std::cout | std::cerr
#include <cstring>   // NULL | strlen | memset
#include <cstdlib>   // NULL | EXIT_FAILURE | EXIT_SUCCESS | size_t | exit
#include <sys/socket.h> // socket | setsockopt | bind | listen | accept | recv | send | close
#include <netinet/in.h> // sockaddr_in | htons | inet_ntoa
#include <arpa/inet.h>  // inet_ntoa
#include <unistd.h>     // close
#include <fcntl.h>      // fcntl

const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1024;
const int PORT = 8080;

// Function to set a socket to non-blocking mode
void set_nonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0); // fcntl is used to manipulate file descriptor flags
    if (flags == -1)
    {
        std::cerr << "Error in fcntl: unable to get flags (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error in fcntl: unable to set non-blocking (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int server_fd, new_socket;
    int client_sockets[MAX_CLIENTS];
    int activity, max_sd, sd;
    int addrlen;
    struct sockaddr_in address; // sockaddr_in is a structure for handling internet addresses
    fd_set readfds; // fd_set is used for the select function to monitor multiple file descriptors
    char buffer[BUFFER_SIZE]; // Buffer to hold data from clients

    // Initialize all client sockets to 0 (not checked in use)
    memset(client_sockets, 0, sizeof(client_sockets)); // memset replaces std::fill for C++98

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET is for IPv4, SOCK_STREAM is for TCP
    if (server_fd == 0)
    {
        std::cerr << "Socket creation failed (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server socket created with fd: " << server_fd << std::endl;

    // Set server socket to reuse address (prevents "Bind failed" error on restart)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) // setsockopt configures socket options
    {
        std::cerr << "setsockopt failed (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set server socket to non-blocking mode
    set_nonblocking(server_fd);

    // Setup server address structure
    address.sin_family = AF_INET; // Address family: IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept any incoming address
    address.sin_port = htons(PORT); // Convert port number to network byte order (big-endian)
    addrlen = sizeof(address);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed: unable to bind to address or port (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server socket bound to port " << PORT << std::endl;

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) // Listen for up to 3 connections in the queue
    {
        std::cerr << "Listen failed (custom error message)" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "DEBUG: Server is listening for connections" << std::endl;

    while (true)
    {
        // Clear the socket set
        FD_ZERO(&readfds); // FD_ZERO clears the set
        FD_SET(server_fd, &readfds); // Add server socket to set
        max_sd = server_fd;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            sd = client_sockets[i]; // File descriptor of the client socket

            if (sd > 0) // Check if socket is valid
            {
                FD_SET(sd, &readfds); // Add valid socket to set
            }

            if (sd > max_sd) // Track the highest file descriptor
            {
                max_sd = sd;
            }
        }

        // Wait for an activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL); // select monitors multiple file descriptors for activity
        if (activity < 0)
        {
            std::cerr << "Select error (custom error message)" << std::endl;
        }

        // If something happened on the server socket, it's an incoming connection
        if (FD_ISSET(server_fd, &readfds))
        {
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen); // accept incoming connection
            if (new_socket < 0)
            {
                std::cerr << "Accept failed (custom error message)" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "DEBUG: New connection, socket fd: " << new_socket
                      << ", IP: " << inet_ntoa(address.sin_addr) // inet_ntoa converts the address from binary to text
                      << ", Port: " << ntohs(address.sin_port) << std::endl; // ntohs converts port number from network byte order to host byte order

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

            if (FD_ISSET(sd, &readfds)) // Check if there is data on the client socket
            {
                int valread = recv(sd, buffer, BUFFER_SIZE, 0); // recv reads data from the socket
                if (valread == 0) // Check if the client disconnected
                {
                    std::cout << "DEBUG: Client disconnected, socket fd: " << sd << std::endl;
                    close(sd); // Close the socket
                    client_sockets[i] = 0; // Mark as available in the client array
                }
                else
                {
                    buffer[valread] = '\0'; // Null-terminate the buffer
                    std::cout << "DEBUG: Received data from client " << i << ": " << buffer << std::endl;

                    // Simple HTTP response
                    const char *response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 13\r\n"
                        "Connection: close\r\n\r\n"
                        "Hello, World!";

                    // Send the response
                    send(sd, response, strlen(response), 0); // send writes data to the socket
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

