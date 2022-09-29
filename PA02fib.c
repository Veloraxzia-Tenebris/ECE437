#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <getopt.h>

// Global variables
int pipee;
pid_t pid;

int fib_seq(int);
void myfib(int, int);

int main(int argc, char* argv[]) {
	printf("Start fib\n");

	int opt, n, m, out;
/*
	// Check for terminal inputs
	while((opt = getopt(argc, argv, ":F:S")) != -1) {
		switch(opt) {
			case 'F':
				n = atoi(optarg);
				break;
			case 'S':
				m = atoi(optarg);
				break;
			default:
				printf("Input error!\n");
				break;
		}
	}

	// Calculate fib
	myfib(n, m);*/
	// Debugging
	myfib(6, 3);
	read(pipee, &out, sizeof(out));
	printf("The result is %d\n", out);
	return 0;
}

int fib_seq(int x) { /* slow/recursive implementation of Fib */
	int i, rint = (rand() % 30);
	double dummy;
	for(i = 0; i < rint * 100; i++) {
		dummy = 2.345 * i * 8.765 / 1.234;
	}
	if(x == 0) {
		return(0);
	} else if(x == 1) {
		return(1);
	} else {
		return(fib_seq(x - 1) + fib_seq(x - 2));
	}
}

void myfib(int n, int m) {
	int msg1, msg2, out;

	// Test for case 2 or case 1
	if(((n - 1) <= m) && ((n - 2) <= m)) {
		printf("Doing slow\n");
		fib_seq(n);
	} else {
		printf("Doing fast\n");

		// Make pipe
		if(mkfifo("data", 0666) < 0 && errno != EEXIST) {
			perror("Named pipe error!");
			exit(1);
		}
		if((pipee = open("data", O_RDWR)) < 0) {
			perror("Named pipe open error!");
			exit(2);
		}

		// Calculate fib recursively
		int temp = n;
		while(temp > 1) {
			pid = fork();
			if(pid == 0) {
				// Child process
				if(temp == 2) {
					// Load fib(0) and fib(1) into the pipe
					int temp1, temp2;
					temp1 = 0;
					temp2 = 1;
					write(pipee, &temp1, sizeof(temp1));
					write(pipee, &temp2, sizeof(temp2));
				}
			} else if(pid < 0) {
				perror("Fork error!");
				exit(3);
			} else {
				// Parent process
				wait(NULL);
				// Read fib(n - 2) and fib(n - 1)
				read(pipee, &msg1, sizeof(msg1));
				read(pipee, &msg2, sizeof(msg2));
				// Debug statement
				printf("msg1: %d and msg2: %d\n", msg1, msg2);
				out = msg1 + msg2;
				// Write fib(n - 1) and fib(n)
				write(pipee, &msg2, sizeof(msg2));
				write(pipee, &out, sizeof(out));
				if(temp == n) {
					return;
				} else {
					exit(0);
				}
			}
			temp--;
		}
	}
	exit(0);
	return;
}