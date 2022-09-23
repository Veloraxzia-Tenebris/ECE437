#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int fib_seq(int);
void myfib(int, int);

int main() {
	printf("Start\n");
	myfib(6, 3);
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
	int stat;
	if(((n - 1) <= m) && ((n - 2) <= m)) {
		printf("Doing slow\n");
		fib_seq(n);
	} else {
		printf("Doing fast\n");
		pid_t pid;
		extern int errno;
		int pipe;
		if(mkfifo("data", 0666) < 0 && errno != EEXIST) {
			perror("Named pipe error!");
			exit(1);
		}
		if((pipe = open("data", O_RDWR)) < 0) {
			perror("Named pipe open error!");
			exit(2);
		}
		int temp1, temp2;
		temp1 = 0;
		temp2 = 1;
		write(pipe, &temp1, sizeof(temp1));
		write(pipe, &temp2, sizeof(temp2));
		int temp = n;
		pid = fork();
		while(temp > 2) {
			if(pid == 0) {
				printf("Parent for %d created.\n", temp);
				waitpid(pid, &stat, 0);
				int msg1, msg2;
				read(pipe, &msg1, sizeof(msg1));
				read(pipe, &msg2, sizeof(msg2));
				printf("Pipe: msg1: %d, msg2: %d\n", msg1, msg2);
				exit(0);
			} else {
				printf("Child for %d created.\n", temp);
				pid = fork();
			}
			temp--;
		}
		if(pid == 0) {
			printf("Parent calculation started for pipe data ");
			int msg1, msg2, out;
			read(pipe, &msg1, sizeof(msg1));
			printf("test");
			read(pipe, &msg2, sizeof(msg2));
			printf("msg1: %d and msg2: %d\n", msg1, msg2);
			out = msg1 + msg2;
			write(pipe, &msg2, sizeof(msg2));
			write(pipe, &out, sizeof(out));
			exit(0);
		}
		return;
	}
}