#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <random>
#include <wait.h>
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
    int semId; // Идентификатор семафора для каждой книги
};

std::vector <Book> library;

class Student {
public:
    int id;
    int checkingSpeed;

    void student_process() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, library.size() - 1);
        int shmid = shmget(IPC_PRIVATE, sizeof(bool), IPC_CREAT | 0666);
        bool *allChecked = (bool *) shmat(shmid, NULL, 0);
        *allChecked = false;

        while (!(*allChecked)) {
            int randomIndex = distrib(gen);
            Book &chosen = library[randomIndex];
            struct sembuf sops;
            sops.sem_op = 0;
            semop(chosen.semId, &sops, 0);
            if (!chosen.checked) {
                printf("...Student %d is checking book #%d...\n", this->id, chosen.id);
                usleep(1000000 * this->checkingSpeed); // Имитация проверки
                if (chosen.taken) {
                    printf("Book #%d from %d line, %d bookcase, %d position is taken!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                } else {
                    printf("Book #%d from %d line, %d bookcase, %d position is on side!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                }
                chosen.checked = true;
                usleep(1500000); // 1.5 seconds waiting
            } else {
                printf("Book #%d is already checked.\n", chosen.id);
            }

            sops.sem_op = 1;
            semop(chosen.semId, &sops, 1);
        }

        printf("ENDDD\n");
        shmdt(allChecked);
        shmctl(shmid, IPC_RMID, 0);
        exit(0);
    }
};

std::vector <Student> students;

void InitStudents(int q) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> st(2, 6);
    for (int i = 0; i < q; ++i) {
        Student cur;
        cur.id = i;
        cur.checkingSpeed = st(gen);
        students.push_back(cur);
    }
}

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

                book.semId = semId;
                library.push_back(book);
            }
        }
    }
}

void DeleteSem(int semId) {
    semctl(semId, 0, IPC_RMID);
}

int main(int argc, char *argv[]) {
    GenerateBooks();
    if (argc != 2) {
        fprintf(stderr, "Incorrect input");
        exit(1);
    }

    int studentsCount = std::stoi(argv[1]);
    InitStudents(studentsCount);
    pid_t pid;

    for (int i = 0; i < studentsCount; ++i) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        } else if (pid == 0) {
            students[i].student_process(); // Дочерний процесс - студент
        }
    }

    int allCheckedShmId = shmget(IPC_PRIVATE, sizeof(bool), IPC_CREAT | 0666);
    bool *allCheckedPtr = (bool *) shmat(allCheckedShmId, NULL, 0);


    shmdt(allCheckedPtr);
    shmctl(allCheckedShmId, IPC_RMID, 0);

    for (int i = 0; i < studentsCount; ++i) {
        wait(NULL);
    }

    while (!*allCheckedPtr) {
        *allCheckedPtr = true;
        for (const auto &book: library) {
            if (!book.checked) {
                *allCheckedPtr = false;
                break;
            }
        }
    }

    for (const auto &book: library) {
        DeleteSem(book.semId);
    }

    return 0;
}
