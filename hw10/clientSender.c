#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080

int main() {
    struct sockaddr_in server_address;
    int sock = 0;
    char *message = "*MESSAGE FROM CLIENT PETYA*";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    connect(sock, (struct sockaddr *) &server_address, sizeof(server_address));
    send(sock, message, strlen(message), 0);
    printf("MESSAGE FROM CLIENT PETYA HAS BEEN SENT!\n");
    return 0;
}