#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

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

int main() {
    int server_fd, new_socket, max_sd, sd, activity, valread;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].socket = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    printf("Server đang lắng nghe trên cổng %d\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            new_socket = accept(server_fd, NULL, NULL);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == 0) {
                    clients[i].socket = new_socket;
                    // send(new_socket, "Nhập tên người dùng của bạn: ", strlen("Nhập tên người dùng của bạn: "), 0);
                    printf("Kết nối mới tại socket %d\n", new_socket);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;
            if (FD_ISSET(sd, &readfds)) {
                valread = read(sd, buffer, BUFFER_SIZE - 1);
                if (valread == 0) {
                    // Client ngắt kết nối
                    sprintf(buffer, "%s đã rời phòng chat.\n", clients[i].username);
                    broadcast_message(buffer, sd);
                    printf("%s", buffer);
                    close(sd);
                    remove_client(i);
                } else {
                    buffer[valread] = '\0';

                    if (strlen(clients[i].username) == 0) {
                        // Nhận tên người dùng
                        strncpy(clients[i].username, buffer, sizeof(clients[i].username) - 1);
                        clients[i].username[strcspn(clients[i].username, "\n")] = '\0';
                        sprintf(buffer, "%s đã tham gia phòng chat.\n", clients[i].username);
                        broadcast_message(buffer, sd);
                        printf("%s", buffer);
                    } else {
                        // Nhận tin nhắn kèm tên người dùng
                        char message_with_name[BUFFER_SIZE + 30];
                        sprintf(message_with_name, "%s: %s", clients[i].username, buffer);
                        broadcast_message(message_with_name, sd);
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
