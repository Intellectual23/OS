#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define PORT 8080
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

typedef struct {
    int socket;
    pthread_t thread;
} Client;

Client clients[5];
int client_counter = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lib_mutex = PTHREAD_MUTEX_INITIALIZER;

void addClient(int socket) {
    pthread_mutex_lock(&client_mutex);
    clients[client_counter].socket = socket;
    client_counter++;
    pthread_mutex_unlock(&client_mutex);
}

void sendMessage(Message *msg) {
    pthread_mutex_lock(&client_mutex);
    // catalogue client is the first client, so his id is 0.
    send(clients[0].socket, msg, sizeof(Message), 0);
    pthread_mutex_unlock(&client_mutex);
}

Book library[M * N * K];
int AllChecked = 0; // status for library if all books are checked

void CheckLibStatus() {
    for (int i = 0; i < M * N * K; ++i) {
        if (library[i].checked == 0) {
            return;
        }
    }
    AllChecked = 1;
}

// Book generation
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
    free(arg);
    Message msg;
    // hello need for server recognition of student, no catalogue
    int hello;
    while (recv(client_socket, &hello, sizeof(int), 0) >= 0 && AllChecked == 0) {

        pthread_mutex_lock(&lib_mutex);

        srand(time(NULL));
        // choosing book
        int randomIndex = rand() % (M * N * K);
        Book book_to_check = library[randomIndex];
        send(client_socket, &book_to_check, sizeof(Book), 0);
        // received data
        recv(client_socket, &msg, sizeof(msg), 0);
        Book checked_book = library[msg.bookId];
        int checked_book_status = msg.taken;
        library[checked_book.id].taken = checked_book_status;
        if (library[checked_book.id].checked == 1) {
            sprintf(msg.text, "Book#%d has already been checked.\n", checked_book.id);
        } else {
            sprintf(msg.text, "Book #%d is at line %d, bookcase %d, position %d. It is %s.\n", checked_book.id,
                    checked_book.line, checked_book.bookCase, checked_book.position,
                    (checked_book.taken ? "taken" : "not taken"));
            library[checked_book.id].checked = 1;
        }
        CheckLibStatus();

        pthread_mutex_unlock(&lib_mutex);

        sendMessage(&msg);

    }
    close(client_socket);
    return NULL;
}

int main() {
    int server_fd, new_socket, valread, *client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    GenerateLibrary();
    printf("LIBRARY IS GENERATED!\n");
    while ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) >= 0) {
        printf("- NEW CONNECTION IS ACCEPTED\n");
        pthread_t thread;
        client_socket = malloc(sizeof(int));
        *client_socket = new_socket;
        addClient(new_socket);
        pthread_create(&thread, NULL, HandleClient, client_socket);
    }
    return 0;
}
