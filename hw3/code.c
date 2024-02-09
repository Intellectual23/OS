#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>

// Calculates the n-th fibonacci number recursive.
unsigned long long int fibonacci(unsigned long long int n) {
	if (n <= 1) {
		return n;
	} 
	return fibonacci(n-1) + fibonacci(n-2);
}

// Calculates the n factorial recursive.
unsigned long long int factorial(unsigned long long int n){
	if(n <= 1) {
		return 1;
	}
	return n * factorial(n-1);
}

// Child process
void child_process(unsigned long long int n){
	printf("Child Process: PID=%d, Parent PID=%d\n", getpid(), getppid());
	// The max value of factorial that can be placed in unsigned long long int is 20!.
	if (n > 20){
		printf("Overflow while calculating factorial!\n");
		return;
	}
	printf("%llu! = %llu\n", n, factorial(n));
}

// Catalog show process
void catalog_process(){
	printf("\nCurrent directory contents:\n");
    DIR *dir;
    struct dirent *entry;
    dir = opendir(".");
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
	unsigned long long int n = strtoull(argv[1], NULL, 10);
	printf("Parent Process: PID=%d, Parent PID=%d\n", getpid(), getppid());
	pid_t pid = fork();
	// Fork case.
	if(pid == -1){
		// Error message.
		perror("Fork");
		return 1;
	}
	// Child process case -> factorial(n);
	else if (pid == 0) {
		child_process(n);
		exit(0);
	}
	// Parent process case -> fibonacci(n) + show catalog;
	else {
		wait(NULL);
		printf("Fib(%llu) = %llu", n, fibonacci(n));
		catalog_process();
	}
	return 0;
}