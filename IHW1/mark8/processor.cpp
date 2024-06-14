#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define FIFO_FIRST "/tmp/fifo_first"
#define BUF_SIZE  5000
#define FIFO_SECOND "/tmp/fifo_second"

bool isLower(char c) {
    return ('a' <= c && c <= 'z');
}

bool isUpper(char c) {
    return ('A' <= c && c <= 'Z');
}

void processing(const char *str, char *res, ssize_t len) {
    int ind = 0;
    bool has_upper = false;
    for (int i = 0; i < len; ++i) {
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

int main() {
    int fifo_first = open(FIFO_FIRST, O_RDONLY); // for request
    int fifo_second = open(FIFO_SECOND, O_WRONLY); // for respond
    if (fifo_second < 0 || fifo_first < 0) {
        exit(0);
    }
    char buf[BUF_SIZE];
    char res[BUF_SIZE];
    ssize_t read_size;
    while ((read_size = read(fifo_first, buf, sizeof(buf))) > 0) {
        processing(buf, res, read_size);
        write(fifo_second, res, strlen(res) + 1);
    }
    close(fifo_first);
    close(fifo_second);
    return 1;
}