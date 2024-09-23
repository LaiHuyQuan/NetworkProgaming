#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_ADDR_STRLEN 128

char *sock_ntop(const struct sockaddr *sa, socklen_t salen) {
    static char str[MAX_ADDR_STRLEN];   // Buffer to hold the string representation
    char portstr[8];                    // Buffer to hold the port as a string

    // Check if the address is IPv4
    if (sa->sa_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *) sa;
        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
            return NULL;   // Return NULL on failure
        }
        snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port)); // Convert port to string
        strcat(str, portstr);   // Append the port to the IP string
        return str;
    }
     else {
        return NULL;  // Unsupported address family
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char ip_str[INET_ADDRSTRLEN];
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind to address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept a connection from the client
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("IPv4 Address: %s\n", sock_ntop((struct sockaddr *) &address, sizeof(address)));

    // Send a response to the client
    if (inet_ntop(AF_INET, &address.sin_addr, ip_str, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop failed");
        exit(EXIT_FAILURE);
    }

    // Send the IP address as a string back to the client
    send(new_socket, ip_str, strlen(ip_str), 0);
    printf("IP address sent to client: %s\n", ip_str);

    // Close the socket
    close(new_socket);
    close(server_fd);

    return 0;
}
