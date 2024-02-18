#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
	// Print error message if data is incorrect.
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
	
	// Input file-descriptor.
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("Error opening input file");
        return 1;
    }
	
	// Output file descriptor.
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (output_fd == -1) {
        perror("Error opening output file");
        close(input_fd);
        return 1;
    }

	// Using buffer with limited size.
    char buffer[BUFFER_SIZE];
	
	// Cycled use for reading and writing.
    ssize_t bytes_read;
	ssize_t bytes_written;
    while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(output_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to output file");
            close(input_fd);
            close(output_fd);
            return 1;
        }
    }
	
	// For working with .bin and .exe files. 
    struct stat st;
    fstat(input_fd, &st);
	// Common text file using common mode.
    if (st.st_mode & S_IXUSR) {
        fchmod(output_fd, st.st_mode);
    }
	
	//Close
    close(input_fd);
    close(output_fd);

    return 0;
}