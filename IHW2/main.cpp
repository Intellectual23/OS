#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <vector>
#include <random>

#define M 3
#define N 4
#define K 5

std::random_device rd;
std::mt19937 gen(rd());

struct Book {
    int id;
    int line;
    int bookCase;
    int position;
    bool taken;
    bool checked;
    sem_t *sem; // Семафор для каждой книги
    char *semName;
};
std::vector <Book> library;

class Student {
public:
    int id;
    int checkingSpeed;

    void student_process() {
        std::uniform_int_distribution<> distrib(0, library.size() - 1);
        while (true) {
            int randomIndex = distrib(gen);
            Book &chosen = library[randomIndex];

            // Захватываем семафор книги
            sem_wait(chosen.sem);

            if (!chosen.checked) {
                printf("...Student %d is checking book #%d...\n", this->id, chosen.id);
                usleep(1000000 * this->checkingSpeed); //имитация проверки
                if (chosen.taken) {
                    printf("Book #%d from %d line, %d bookcase, %d position is taken!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                } else {
                    printf("Book #%d from %d line, %d bookcase, %d position is on side!\n",
                           chosen.id, chosen.line, chosen.bookCase, chosen.position);
                }
                chosen.checked = true;
                usleep(1500000); //1.5 seconds waiting.
            } else {
                printf("Book #%d is already checked.\n", chosen.id);
            }

            // Отпускаем семафор книги
            sem_post(chosen.sem);

            // Проверяем, все ли книги проверены
            bool allChecked = true;
            for (const auto &book: library) {
                if (!book.checked) {
                    allChecked = false;
                    break;
                }
            }

            if (allChecked) {
                break; // Все книги проверены, завершаем работу
            }
        }
    }
};

std::vector <Student> students;

void InitStudents(int q) {
    std::uniform_int_distribution<> st(2, 6);
    for (int i = 0; i < q; ++i) {
        Student cur;
        cur.id = i;
        cur.checkingSpeed = st(gen);
        students.emplace_back(cur);
    }
}

void GenerateBooks() {
    int id = 1;
    std::uniform_int_distribution<> dist(0, 5);
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

                // Создаем семафор для книги
                char name[15];
                sprintf(name, "book_sem_%d", book.id);
                book.semName = name;
                book.sem = sem_open(book.semName, O_CREAT, 0666, 1);
                if (book.sem == SEM_FAILED) {
                    perror("sem_open");
                    exit(1);
                }

                library.emplace_back(book);
            }
        }
    }
}

void DeleteSem(sem_t *sem, char *name) {
    sem_close(sem); // Закрываем семафор
    sem_unlink(name); // Удаляем семафор
}

int main(int argc, char *argv[]) {

    GenerateBooks();

    if (argc != 2) {
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    int studentsCount = atoi(argv[1]);
    InitStudents(studentsCount);

    pid_t pid;
    for (int i = 0; i < studentsCount; ++i) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork failed");
            return 1;
        } else if (pid == 0) {
            students[i].student_process(); // Дочерний процесс - студент
            exit(0);
        }
    }

    for (int i = 0; i < studentsCount; ++i) {
        wait(NULL); // Ждем завершения всех дочерних процессов
    }

    for (int i = 0; i < library.size(); ++i) {
        DeleteSem(library[i].sem, library[i].semName);
    }

    return 0;
}