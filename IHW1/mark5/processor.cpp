#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <cstring>

#define FIFO_READ "/tmp/fifo_reader"
#define BUF_SIZE  5000
#define FIFO_WRITE "/tmp/fifo_writer"

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

int main() {
    int input_fd = open(FIFO_READ, O_RDONLY);

    if (input_fd <= -1) {
        fprintf(stderr, "Error opening file!\n");
        exit(0);
    }

    int out_fd = open(FIFO_WRITE, O_WRONLY);

    if (out_fd <= -1) {
        fprintf(stderr, "Error opening file!\n");
        exit(0);
    }
    char input_data[BUF_SIZE] = {0};
    char output_data[BUF_SIZE] = {0};
    int read_size = read(input_fd, input_data, BUF_SIZE);
    if (read_size > 0) {
        processing(input_data, output_data);
        sprintf(output_data,"\n");
        write(out_fd, output_data, strlen(output_data) + 1);
    }

    close(input_fd);
    close(out_fd);

    return 0;
}

