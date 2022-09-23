#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int state = 0;
pid_t pid;

void signalGet(int sig) {
	if(sig == SIGINT) {
		printf("SIGINT caught, killing child process...\n");
		kill(pid, SIGKILL);
		printf("Child has been killed\n");
		exit(0);
	} else if(sig == SIGTSTP) {
		state++;
		if(state % 2 == 0) {
			printf("Ctrl-Z caught, stopping child process...\n");
			kill(pid, SIGSTOP);
			printf("Child has been stopped\n");
			exit(0);
		} else {
			printf("Ctrl-Z caught, resuming child process...\n");
			kill(pid, SIGCONT);
			printf("Child has been resumed\n");
			exit(0);
		}
	}
}

int main() {
	signal(SIGINT, signalGet);
	signal(SIGTSTP, signalGet);
	pid = fork();
	if(pid == 0) {
		printf("Child initialized\n");
		execv("/usr/bin/yes", NULL);
	} else {
		printf("Parent initialized\n");
	}
	while(1) {
		sleep(1);
	}
	return(0);
}