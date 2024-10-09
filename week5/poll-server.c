#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 100
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    char username[30];
} Client;

Client clients[MAX_CLIENTS];

void broadcast_message(char *message, int sender_socket);
void remove_client(int index);
void handle_sigint(int sig) {
    printf("\nServer đang dừng lại...\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint); // Để xử lý khi dừng server

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].socket = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    printf("Server đang lắng nghe trên cổng %d\n", PORT);

    struct pollfd fds[MAX_CLIENTS + 1]; // Thêm 1 cho server_fd
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    while (1) {
        for (int i = 1; i <= MAX_CLIENTS; i++) {
            fds[i].fd = clients[i - 1].socket;
            fds[i].events = POLLIN;
        }

        int activity = poll(fds, MAX_CLIENTS + 1, 5000); // Thời gian chờ 5 giây

        if (activity < 0) {
            perror("poll");
        }

        if (fds[0].revents & POLLIN) {
            new_socket = accept(server_fd, NULL, NULL);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == 0) {
                    clients[i].socket = new_socket;
                    printf("Kết nối mới tại socket %d\n", new_socket);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].socket != 0 && (fds[i + 1].revents & POLLIN)) {
                valread = read(clients[i].socket, buffer, BUFFER_SIZE - 1);
                if (valread == 0) {
                    sprintf(buffer, "%s đã rời phòng chat.\n", clients[i].username);
                    broadcast_message(buffer, clients[i].socket);
                    printf("%s", buffer);
                    close(clients[i].socket);
                    remove_client(i);
                } else {
                    buffer[valread] = '\0';

                    if (strlen(clients[i].username) == 0) {
                        strncpy(clients[i].username, buffer, sizeof(clients[i].username) - 1);
                        clients[i].username[strcspn(clients[i].username, "\n")] = '\0';
                        sprintf(buffer, "%s đã tham gia phòng chat.\n", clients[i].username);
                        broadcast_message(buffer, clients[i].socket);
                        printf("%s", buffer);
                    } else {
                        char message_with_name[BUFFER_SIZE + 30];
                        sprintf(message_with_name, "%s: %s", clients[i].username, buffer);
                        broadcast_message(message_with_name, clients[i].socket);
                    }
                }
            }
        }
    }
    close(server_fd);
    return 0;
}

void broadcast_message(char *message, int sender_socket) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != sender_socket && clients[i].socket != 0) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
}

void remove_client(int index) {
    clients[index].socket = 0;
    memset(clients[index].username, 0, sizeof(clients[index].username));
}
