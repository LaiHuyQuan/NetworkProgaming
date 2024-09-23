#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char ip_str[INET_ADDRSTRLEN];
    char ip_str_rep[INET_ADDRSTRLEN];
    printf("Enter IP address: \n");
    scanf("%s", ip_str);

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP addresses from text to binary
     if (inet_pton(AF_INET, ip_str, &serv_addr.sin_addr) == 1){
        printf("inet_pton (IPv4): Successfully converted IP address: %s\n", ip_str);
    } else {
        printf("inet_pton (IPv4): Failed to convert IP address: %s\n", ip_str);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    // Read server's response
    read(sock, ip_str_rep, INET_ADDRSTRLEN);
    printf("Message from server: %s\n", ip_str_rep);

    // Close the socket
    close(sock);

    return 0;
}

