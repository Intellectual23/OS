#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include <vector>
#include <random>
#include <cstring>

#define M 3
#define N 4
#define K 5

struct Book {
    int id;
    int line;
    int bookCase;
    int position;
    bool taken;
    bool checked;
    int semId;
};

std::vector <Book> library;

void GenerateBooks() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 5);
    int id = 1;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < K; ++k) {
                Book book;
                book.id = id++;
                book.line = i;
                book.bookCase = j;
                book.position = k;
                book.checked = false;
                int t = dist(gen);
                if (t % 2 == 0) {
                    book.taken = true;
                } else {
                    book.taken = false;
                }

                int semId = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
                if (semId == -1) {
                    perror("semget");
                    exit(1);
                }

                // Инициализируем семафор
                semctl(semId, 0, SETVAL, 1);

                book.semId = semId;
                library.push_back(book);
            }
        }
    }
}

int main() {
    GenerateBooks();
    int shmid = shmget(IPC_PRIVATE, sizeof(library), IPC_CREAT | 0666);
    Book *books = (Book *) shmat(shmid, NULL, 0);
    memcpy(books, library.data(), library.size() * sizeof(Book));
    printf("All books generated!\n");
    // Ждем завершения всех студентов
    for (int i = 0; i < library.size(); ++i) {
        struct sembuf sops;
        sops.sem_num = 0;
        sops.sem_op = 1;
        sops.sem_flg = 0;
        semop(books[i].semId, &sops, 1);
    }

    while (true) {
        bool allChecked = true;
        for (const auto &book: library) {
            if (!book.checked) {
                allChecked = false;
                break;
            }
        }
        if (allChecked) {
            break;
        }
    }

    shmdt(books);
    shmctl(shmid, IPC_RMID, 0);

    for (const auto &book: library) {
        semctl(book.semId, 0, IPC_RMID); // Удаляем семафоры
    }

    return 0;
}
