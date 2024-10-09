#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_QUESTIONS 10

// Cấu trúc để lưu trữ câu hỏi trắc nghiệm
typedef struct {
    char question[256];
    char options[4][100];
    int correct_option; // Chỉ số của đáp án đúng (0-3)
} Question;

// Danh sách 10 câu hỏi
Question questions[MAX_QUESTIONS] = {
    {"Màu sắc của bầu trời là gì?", {"Xanh","Đỏ", "Vàng", "Trắng"}, 0},
    {"Thủ đô của Việt Nam là gì?", {"Hà Nội", "TP Hồ Chí Minh", "Đà Nẵng", "Cần Thơ"}, 0},
    {"Ngày Quốc khánh Việt Nam là ngày nào?", {"2/9", "1/1", "30/4", "20/11"}, 0},
    {"Hệ hành tinh của chúng ta có bao nhiêu hành tinh?", {"8", "9", "10", "7"}, 0},
    {"Ai là tác giả của 'Truyện Kiều'?", {"Nguyễn Du", "Tố Hữu", "Nam Cao", "Xuân Diệu"}, 0},
    {"Điện thoại di động đầu tiên được ra mắt vào năm nào?", {"1983", "1990", "2000", "2010"}, 0},
    {"Nước nào có diện tích lớn nhất thế giới?", {"Nga", "Canada", "Mỹ", "Trung Quốc"}, 0},
    {"Hạt nhân của nguyên tử gồm những phần nào?", {"Proton và Neutron", "Electron và Proton", "Electron và Neutron", "Chỉ Proton"}, 0},
    {"Hành tinh nào gần Mặt trời nhất?", {"Kim Tinh", "Venus", "Trái Đất", "Hỏa Tinh"}, 0},
    {"Đội bóng nào đã vô địch World Cup 2018?", {"Pháp", "Brazil", "Đức", "Argentina"}, 0}
};

// Hàm xử lý tín hiệu để ngăn ngừa tiến trình zombie
void sigchld_handler(int sig) {
    (void)sig; // Bỏ qua cảnh báo tham số không sử dụng
    while (waitpid(-1, NULL, WNOHANG) > 0); // Chờ và thu hồi các tiến trình con
}

void handle_client(int connfd) {
    char buffer[BUFFER_SIZE];
    int n, answer;
    int score = 0; // Biến để theo dõi số điểm

    // Nhận tín hiệu từ client để bắt đầu gửi câu hỏi
    n = recv(connfd, buffer, BUFFER_SIZE, 0);
    if (n <= 0) {
        perror("recv failed");
        close(connfd);
        return;
    }
    buffer[n] = '\0';

    if (strncmp(buffer, "GET_QUESTIONS", 14) == 0) {
        // Gửi câu hỏi cho client
        for (int i = 0; i < MAX_QUESTIONS; i++) {
            snprintf(buffer, BUFFER_SIZE, "%s\n1. %s\n2. %s\n3. %s\n4. %s\n",
                     questions[i].question,
                     questions[i].options[0],
                     questions[i].options[1],
                     questions[i].options[2],
                     questions[i].options[3]);
            send(connfd, buffer, strlen(buffer), 0);

            // Nhận câu trả lời từ client
            n = recv(connfd, buffer, BUFFER_SIZE, 0);
            if (n <= 0) {
                perror("recv failed");
                close(connfd);
                return;
            }
            buffer[n] = '\0'; // Kết thúc chuỗi nhận được
            answer = atoi(buffer); // Chuyển đổi chuỗi thành số nguyên

            // Kiểm tra đáp án và gửi tình trạng về client
            if (answer - 1 == questions[i].correct_option) {
                snprintf(buffer, BUFFER_SIZE, "Bạn đã trả lời đúng câu hỏi %d!\n", i + 1);
                score++; // Tăng điểm khi trả lời đúng
            } else {
                snprintf(buffer, BUFFER_SIZE, "Bạn đã trả lời sai câu hỏi %d. Đáp án đúng là %d: %s\n",
                         i + 1, questions[i].correct_option + 1, questions[i].options[questions[i].correct_option]);
            }
            send(connfd, buffer, strlen(buffer), 0);
        }

        // Gửi số điểm cuối cùng về client
        snprintf(buffer, BUFFER_SIZE, "Bạn đã hoàn thành! Số điểm của bạn là: %d/%d\n", score, MAX_QUESTIONS);
        send(connfd, buffer, strlen(buffer), 0);
    }

    // Đóng socket của client
    close(connfd);
}

int main() {
    int listenfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pid_t pid;

    // Tạo socket lắng nghe
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Thiết lập cấu trúc địa chỉ server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Chấp nhận kết nối từ mọi địa chỉ
    server_addr.sin_port = htons(PORT); // Chuyển đổi cổng sang dạng mạng

    // Gán socket lắng nghe vào cổng đã chỉ định
    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // Lắng nghe các kết nối đến
    if (listen(listenfd, 5) < 0) {
        perror("Listen failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    // Xử lý tín hiệu SIGCHLD để ngăn ngừa tiến trình zombie
    signal(SIGCHLD, sigchld_handler);

    printf("Server is listening on port %d...\n", PORT);

    // Vòng lặp server để chấp nhận nhiều client
    while (1) {
        // Chấp nhận một kết nối đến
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &addr_len);
        if (connfd < 0) {
            perror("Accept failed");
            continue;
        }

        // Tạo tiến trình con để xử lý client
        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(connfd);
        } else if (pid == 0) {
            // Tiến trình con: xử lý client
            close(listenfd);  // Đóng socket lắng nghe trong tiến trình con
            handle_client(connfd);
            exit(0);
        } else {
            // Tiến trình cha: tiếp tục chấp nhận các client mới
            close(connfd);  // Đóng socket client trong tiến trình cha
        }
    }

    return 0;
}
