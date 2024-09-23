#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>

int main(){
    const char *ip_str = "192.168.10.2";

    struct sockaddr_in server_addr;           // Structure to hold the IP address in binary form


    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;

    server_addr.sin_port= htons(8080);

    if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    printf("sockaddr_in structure initialized:\n");
    printf("  Family      : AF_INET (IPv4)\n");
    printf("  Port        : %d\n", ntohs(server_addr.sin_port));  // convert back to host byte order for printing
    printf("  IP Address  : %s\n", inet_ntoa(server_addr.sin_addr));

    return 0;
}