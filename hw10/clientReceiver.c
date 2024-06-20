#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    int sock = 0;
    int valread;
    struct sockaddr_in server_address;
    char buf[1024] = {0};
    if (argc != 3) {
        fprintf(stderr, "Incorrect input\n");
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_address.sin_addr) <= 0) {
        return 1;
    }

    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        return 1;
    }

    while (1) {
        valread = read(sock, buf, 1024);
        if (valread > 0) {
            printf("RECEIVEDD MESSAGE %s\n", buf);
            if (strcmp(buf, "Finish") == 0) break;
            memset(buf, 0, 1024);
        }
    }

    close(sock);
    return 0;
}