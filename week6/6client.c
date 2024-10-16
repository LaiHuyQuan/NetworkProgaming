#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 100
#define SECRET_KEY 'K' // Khóa để mã hóa

// Hàm mã hóa và giải mã dữ liệu bằng XOR
void xor_cipher(char *data, char key) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= key;
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;

    // Tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Thiết lập thông tin server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Kết nối đến server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Kết nối đến server thành công.\n");

    // Gửi tên người dùng
    printf("Nhập tên của bạn: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    send(sockfd, buffer, strlen(buffer), 0);

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sockfd, &read_fds);
        int max_fd = sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            break;
        }

        // Kiểm tra input từ bàn phím
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            // Nhập tin nhắn
            fgets(buffer, sizeof(buffer), stdin);
            xor_cipher(buffer, SECRET_KEY); // Mã hóa trước khi gửi
            send(sockfd, buffer, strlen(buffer), 0);
        }

        // Kiểm tra tin nhắn từ server
        if (FD_ISSET(sockfd, &read_fds)) {
            int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
            if (bytes_received <= 0) {
                printf("Mất kết nối đến server.\n");
                break;
            }
            buffer[bytes_received] = '\0';
            // Giải mã dữ liệu nhận được
            xor_cipher(buffer, SECRET_KEY);
            printf("%s", buffer); // Hiển thị tin nhắn đã giải mã
        }
    }

    close(sockfd);
    return 0;
}
