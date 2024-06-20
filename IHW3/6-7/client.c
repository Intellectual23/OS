#include <stdlib.h>
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

void student_work(int client_socket) {
    Book book_to_check;
    Message msg;
    int hello = 1;
    send(client_socket, &hello, sizeof(int), 0);
    recv(client_socket, &book_to_check, sizeof(Book), 0);
    //printf("Book received\n");
    if (book_to_check.taken == 1) {
        //printf("%d\n", book_to_check.id);
        msg.taken = 1;
    } else {
        //printf("%d\n", book_to_check.id);
        msg.taken = 0;
    }
    msg.bookId = book_to_check.id;
    send(client_socket, &msg, sizeof(msg), 0);
    usleep(1000000);
}

int main() {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    printf("- CLIENT CONNECTED TO SERVER\n");
    while (1) {
        student_work(sock);
    }

    close(sock);

    return 0;
}
