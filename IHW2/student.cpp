#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <wait.h>

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

class Student {
public:
    int id;
    int checkingSpeed;

    void student_process(const Book *books, int booksCount) {
        int shmid = shmget(IPC_PRIVATE, sizeof(bool), IPC_CREAT | 0666);
        bool *allChecked = (bool *) shmat(shmid, NULL, 0);
        *allChecked = false;

        while (!(*allChecked)) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, M * N * K - 1);
            int randomIndex = distrib(gen);
            Book chosen = books[randomIndex];

            struct sembuf sops;
            sops.sem_num = 0;
            sops.sem_op = -1; // Дожидаемся доступности семафора
            sops.sem_flg = 0;
            semop(chosen.semId, &sops, 1);

            if (!chosen.checked) {
                printf("...Student %d is checking book #%d...\n", this->id, chosen.id);
                usleep(1000000 * this->checkingSpeed);
                if (chosen.taken) {
                    printf("Book #%d from %d line, %d bookcase, %d position is taken!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                } else {
                    printf("Book #%d from %d line, %d bookcase, %d position is on side!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                }
                chosen.checked = true;
                usleep(1500000);
            } else {
                printf("Book #%d is already checked.\n", chosen.id);
            }

            sops.sem_op = 1; // Освобождаем семафор
            semop(chosen.semId, &sops, 1);
        }

        shmdt(allChecked);
        shmctl(shmid, IPC_RMID, 0);
        exit(0);
    }
};

int main() {
    int shmid = shmget(IPC_PRIVATE, sizeof(Book) * M * N * K, IPC_CREAT | 0666);
    Book *books = (Book *) shmat(shmid, NULL, 0);
    pid_t pid;
    int studentsCount = 3; // Количество студентов
    for (int i = 0; i < studentsCount; ++i) {
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return 1;
        } else if (pid == 0) {
            Student student;
            student.id = i;
            student.checkingSpeed = 3; // Скорость проверки книг
            student.student_process(books, M * N * K); // Передаем указатель на массив книг и его размер
        }
    }

    for (int i = 0; i < studentsCount; ++i) {
        wait(NULL);
    }

    return 0;
}
