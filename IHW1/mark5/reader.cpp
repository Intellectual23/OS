#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_READ "/tmp/fifo_reader"
#define BUF_SIZE  5000

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    int input_fd = open(argv[1], O_RDONLY);

    if(input_fd == -1){
        fprintf(stderr, "Error opening file!\n");
        exit(0);
    }

    int fifo_fd = open(FIFO_READ, O_WRONLY);
    if(fifo_fd == -1){
        exit(0);
    }

    char *buf[BUF_SIZE];
    int read_size = read(input_fd, buf, BUF_SIZE);
    if(read_size){
        write(fifo_fd, buf,read_size);
    }

    close(input_fd);
    close(fifo_fd);

    return 0;
}