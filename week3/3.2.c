#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>

void binaryConvert(unsigned char byte){
    for(int i =7; i > 0; i--){
        printf("%d", (byte >> i) & 1);
    }

    printf(" ");
}

int main(){
    char ip_str[INET_ADDRSTRLEN];
    struct in_addr ip_addr;
    printf("Enter IP address: \n");
    scanf("%s", ip_str);
    
    if (inet_pton(AF_INET, ip_str, &ip_addr) == 1){
        printf("inet_pton (IPv4): Successfully converted IP address: %s\n", ip_str);
        printf("Binary converted IP address: ");
        char *bytes = (char *)&ip_addr;
        for (int i =0; i < 4; i++){
            binaryConvert(bytes[i]);
        }
    } else {
        printf("inet_pton (IPv4): Failed to convert IP address: %s\n", ip_str);
        exit(EXIT_FAILURE);
    }

    char ip_str_converted[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &ip_addr, ip_str_converted, INET_ADDRSTRLEN)){
        printf("\ninet_ntop (IPv4): Converted back to string IP address: %s\n", ip_str_converted);
    } else {
        printf("inet_ntop (IPv4): Failed to convert IP address back to string\n");
        exit(EXIT_FAILURE);
    }
    
}