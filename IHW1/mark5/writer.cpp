#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define BUF_SIZE  5000
#define FIFO_WRITE "/tmp/fifo_writer"

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr, "Incorrect input!\n");
        exit(0);
    }

    int outputFd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(outputFd == -1){
        fprintf(stderr, "Error opening file!\n");
        exit(0);
    }

    int fifo_fd = open(FIFO_WRITE, O_RDONLY);
    if(fifo_fd == -1){
        exit(0);
    }

    char buf[BUF_SIZE];
    int read_size;

    while((read_size = read(fifo_fd, buf, BUF_SIZE) > 0)){
        write(outputFd, buf,read_size);
    }

    close(fifo_fd);
    close(outputFd);

    return 0;
}