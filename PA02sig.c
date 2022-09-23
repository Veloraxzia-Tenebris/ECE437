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
		printf("\nSIGINT caught, killing child process...\n");
		kill(pid, SIGKILL);
		printf("Child has been killed\n");
		exit(0);
	} else if(sig == SIGTSTP) {
		if(state == 0) {
			printf("\nCtrl-Z caught, stopping child process...\n");
			kill(pid, SIGSTOP);
			printf("Child has been stopped\n");
			state = 1;
		} else {
			printf("\nCtrl-Z caught, resuming child process...\n");
			kill(pid, SIGCONT);
			printf("Child has been resumed\n");
			state = 0;
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
		sleep(100);
	}
	return(0);
}