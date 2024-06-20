#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define PORT 8080

int main() {
    int server_fd;
    int new_socket;
    int valread;
    struct sockaddr_in server_address;
    int opt = 1;
    int addrlen = sizeof(server_address);
    char buf[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    listen(server_fd, 3);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *) &server_address, (socklen_t *)&addrlen);
        if (new_socket < 0) {
            continue; // Пропускаем ошибочные соединения
        }

        valread = read(new_socket, buf, 1024);
        printf("MESSAGE FROM CLIENT: %s", buf);
        if (strcmp(buf, "Finish") == 0) {
            break;
        }

        close(new_socket);
    }

    close(server_fd);

    return 0;
}
