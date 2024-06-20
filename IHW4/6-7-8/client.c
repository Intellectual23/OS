#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define M 3
#define N 4
#define K 5

typedef struct {
    int id;
    int line;
    int bookCase;
    int position;
    int taken;
    int checked;
} Book;

typedef struct {
    int bookId;
    int taken;
    char text[100];
} Message;
int allChecked = 0;

void student_work(int client_socket, struct sockaddr_in *address, socklen_t len) {
    Book book_to_check;
    Message msg;
    int hello = 1;
    sendto(client_socket, &hello, sizeof(int), 0, (const struct sockaddr *) &address, &len);
    recvfrom(client_socket, &book_to_check, sizeof(Book), 0, (struct sockaddr *) &address, len);
    if (book_to_check.taken == 1) {
        msg.taken = 1;
    } else {
        msg.taken = 0;
    }
    msg.bookId = book_to_check.id;
    sendto(client_socket, &msg, sizeof(msg), 0, (const struct sockaddr *) &address, &len);
    usleep(1000000);
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(serv_addr);

    printf("- CONNECTING TO SERVER");
    student_work(sock, &serv_addr, len);

    printf("- CLIENT CONNECTED TO SERVER");

    close(sock);

    return 0;
}