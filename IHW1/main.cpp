#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const int BUF_SIZE = 5000;

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

    int pipe_fd[2];
    pid_t pid;

    char buf[BUF_SIZE] = {0};
    char proc[BUF_SIZE] = {0};

    if (argc != 2) {
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    if (pipe(pipe_fd) == -1) {
        perror("pipa");
        exit(0);
    }

    pid = fork();
    if (pid == -1) {
        exit(0);
    }

    if (pid == 0) {
        close(pipe_fd[1]);
        read(pipe_fd[0], buf, BUF_SIZE);
        processing(buf, proc);
        printf("%s", proc);
        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fd[0]);
        int fd = open(argv[1], O_RDONLY);
        if (fd == -1) {
            perror("opening");
            exit(0);
        }

        ssize_t read_num = read(fd, buf, BUF_SIZE);
        if (read_num > 0) {
            write(pipe_fd[1], buf, read_num);
        } else {
            write(pipe_fd[1], buf, 1);
        }
        close(fd);
        close(pipe_fd[1]);
        wait(0);
    }
    return EXIT_SUCCESS;
}