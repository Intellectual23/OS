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

void catalogue_work(Message *msg) {
    printf("%s", msg->text);
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len = sizeof(serv_addr);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }
    printf("- MONITOR CONNECTED TO SERVER\n");

    Message msg;

    while (1) {
        recvfrom(sock, &msg, sizeof(msg), 0, (struct sockaddr *) &serv_addr, &len);
        catalogue_work(&msg);
    }

    close(sock);
    return 0;
}
