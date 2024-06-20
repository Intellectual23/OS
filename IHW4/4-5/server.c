#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

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
} Message;

Book library[M * N * K];
int AllChecked = 0; // status for library if all books are checked

void PrintBookStatus(int bookId) {
    Book book = library[bookId];
    printf("Book #%d is at line %d, bookcase %d, position %d. It is %s.\n", book.id, book.line, book.bookCase,
           book.position, (book.taken ? "taken" : "not taken"));
}

void PrintCheckedStatus(int bookId) {
    printf("Book #%d has already been checked!\n", bookId);
}

void CheckLibStatus() {
    for (int i = 0; i < M * N * K; ++i) {
        if (library[i].checked == 0) {
            return;
        }
    }
    AllChecked = 1;
}

// Генерация книг в библиотеке
void GenerateLibrary() {
    srand(time(0));
    int id = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 5; ++k) {
                library[id].id = id;
                library[id].line = i;
                library[id].bookCase = j;
                library[id].position = k;
                library[id].checked = 0;
                library[id].taken = (rand() % 2 == 0);
                ++id;
            }
        }
    }
}

void *HandleClient(void *arg) {
    int client_socket = *(int *) arg;
    struct sockaddr_in address = {0};
    socklen_t len = sizeof(address);
    Message msg;
    if (recvfrom(client_socket, &address, sizeof(address), 0, (struct sockaddr *) &address, &len) < 0) {
        perror("Recvfrom failed");
        close(client_socket);
        return NULL;
    }
    while (AllChecked == 0) {
        srand(time(NULL));
        int randomIndex = rand() % (M * N * K);
        Book book_to_check = library[randomIndex];
        sendto(client_socket, &book_to_check, sizeof(Book), 0, (const struct sockaddr *) &address, len);
        if ((recvfrom(client_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &address, &len) > 0)) {
            Book checked_book = library[msg.bookId];
            int checked_book_status = msg.taken;
            library[checked_book.id].taken = checked_book_status;
            if (library[checked_book.id].checked == 1) {
                PrintCheckedStatus(checked_book.id);
            } else {
                PrintBookStatus(checked_book.id);
                library[checked_book.id].checked = 1;
            }
        }
        CheckLibStatus();
    }
    close(client_socket);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (const struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    GenerateLibrary();
    printf("LIBRARY IS GENERATED!\n");

    //printf("- NEW CONNECTION IS ACCEPTED\n");
    pthread_t thread;
    pthread_create(&thread, NULL, HandleClient, &server_fd);
    pthread_join(thread, NULL);

    close(server_fd);
    return 0;
}
