#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_FIRST "/tmp/fifo_first"
#define BUF_SIZE  5000
#define FIFO_SECOND "/tmp/fifo_second"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    int fifo_first = open(FIFO_FIRST, O_WRONLY);
    if (fifo_first == -1) {
        exit(0);
    }

    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
        exit(0);
    }
    char buf[BUF_SIZE];
    ssize_t read_size;
    while ((read_size = read(input, buf, sizeof(buf))) > 0) {
        if (write(fifo_first, buf, read_size) != read_size) {
            exit(0);
        }
    }
    close(input);
    close(fifo_first);

    int fifo_second = open(FIFO_SECOND, O_RDONLY);
    if (fifo_second == -1) {
        exit(0);
    }
    int output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (output == -1) {
        exit(0);
    }
    //fprintf(stderr, "%s", buf);
    while ((read_size = read(fifo_second, buf, sizeof(buf))) > 0) {
        if (write(output, buf, read_size) != read_size) {
            exit(0);
        }
    }

    close(output);
    close(fifo_second);
    return 1;
}