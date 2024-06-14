#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

#define BUF_SIZE 5000

bool isLower(char c) {
    return ('a' <= c && c <= 'z');
}

bool isUpper(char c) {
    return ('A' <= c && c <= 'Z');
}

void processing(const char *str, char *res) {
    int ind = 0;
    bool has_upper = false;
    for (int i = 0; i < BUF_SIZE; ++i) {
        if (!isLower(str[i]) && !isUpper(str[i])) {
            if (has_upper) {
                has_upper = false;
                res[ind] = ' ';
                ++ind;
            }
        } else if (isUpper(str[i])) {
            if (!has_upper) {
                has_upper = true;
                res[ind] = str[i];
                ++ind;
            } else {
                res[ind] = str[i];
                ++ind;
            }
        } else {
            if (has_upper) {
                res[ind] = str[i];
                ++ind;
            }
        }
    }

    res[ind] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    char buf[BUF_SIZE] = {0};
    pid_t pid;
    int pipe_fd[2];
    int pipe_fd2[2];
    char res[BUF_SIZE] = {0};

    if (pipe(pipe_fd) == -1 || pipe(pipe_fd2) == -1) {
        exit(0);
    }

    pid = fork();

    if (pid == -1) {
        exit(0);
    }

    if (pid == 0) {
        close(pipe_fd[1]);
        close(pipe_fd2[0]);
        read(pipe_fd[0], buf, BUF_SIZE);
        processing(buf, res);
        write(pipe_fd2[1], res, strlen(res) + 1);
        close(pipe_fd[0]);
        close(pipe_fd2[1]);
        exit(1);
    } else {
        close(pipe_fd[0]);
        close(pipe_fd2[1]);
        int input = open(argv[1], O_RDONLY);
        if (input == -1) {
            exit(0);
        }
        ssize_t read_size = read(input, buf, BUF_SIZE);

        if (read_size > 0) {
            write(pipe_fd[1], buf, read_size);
        } else {
            write(pipe_fd[1], buf, 1);
        }

        close(input);
        close(pipe_fd[1]);
        wait(0);

        int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output == -1) {
            exit(0);
        }
        read(pipe_fd2[0], res, sizeof(res)); // because strlen eq 0.
        write(output, res, strlen(res));
        close(output);
        close(pipe_fd2[0]);
    }
    return EXIT_SUCCESS;
}