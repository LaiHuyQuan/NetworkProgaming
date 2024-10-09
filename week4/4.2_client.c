#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int n;

    // Tạo socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập cấu trúc địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IPv4 từ dạng văn bản sang dạng nhị phân
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Kết nối tới server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Requesting questions...\n");

    // Gửi yêu cầu để nhận câu hỏi
    const char *request = "GET_QUESTIONS"; // Tín hiệu yêu cầu nhận câu hỏi
    send(client_socket, request, strlen(request), 0);

    // Nhận câu hỏi từ server
    while (1) {
        n = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (n > 0) {
            buffer[n] = '\0'; // Kết thúc chuỗi nhận được
            printf("Server: %s\n", buffer);
            // Nhận câu trả lời từ người dùng
            printf("Enter your answer: ");
            fgets(buffer, BUFFER_SIZE, stdin);

            // Xóa ký tự xuống dòng nếu có
            if (buffer[0] != '\0' && buffer[strlen(buffer) - 1] == '\n') {
                buffer[strlen(buffer) - 1] = '\0';
            }

            // Gửi câu trả lời đến server
            send(client_socket, buffer, strlen(buffer), 0);
        } else if (n == 0) {
            printf("Server disconnected.\n");
            break;
        } else {
            perror("recv failed");
            break;
        }
    }

    // Đóng socket
    close(client_socket);

    return 0;
}
