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

int M, N, K;
sem_t *p_sem1;

struct Book {
    int id;
    int line;
    int bookCase;
    int position;
    bool taken;
    bool checked;
};

std::vector <Book> library;

void GenerateBooks() {
    int id = 1;
    std::random_device rd;
    std::mt19937 gen(rd());
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
                library.emplace_back(book);
            }
        }
    }
}

void student_process(int id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, library.size() - 1);
    while (true) {
        int randomIndex = distrib(gen);
        sem_wait(p_sem1);
        if (!library[randomIndex].checked) {
            printf("Student %d is checking book with id %d at line %d, bookcase %d, position %d\n", id,
                   library[randomIndex].id, library[randomIndex].line, library[randomIndex].bookCase,
                   library[randomIndex].position);
            usleep(3000000); //имитация проверки
            library[randomIndex].checked = true;
        }
        sem_post(p_sem1);
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

int main() {
    M = 3; // Количество строк
    N = 4; // Количество книжных шкафов в строке
    K = 5; // Количество позиций в каждом книжном шкафу

    GenerateBooks();

    p_sem1 = sem_open("/my_semaphore", O_CREAT, 0666, 1); // Создаем семафор

    int num_students = 5; // Количество студентов

    pid_t pid;
    for (int i = 0; i < num_students; ++i) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork failed");
            return 1;
        } else if (pid == 0) {
            student_process(i); // Дочерний процесс - студент
            exit(0);
        }
    }

    for (int i = 0; i < num_students; ++i) {
        wait(NULL); // Ждем завершения всех дочерних процессов
    }

    sem_close(p_sem1); // Закрываем семафор
    sem_unlink("/my_semaphore"); // Удаляем семафор

    return 0;
}