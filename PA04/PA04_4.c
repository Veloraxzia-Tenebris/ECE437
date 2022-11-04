#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
// Added to use the kill function
#include <signal.h>
#include <semaphore.h>
// Shared memory size for bank
#define SHMSZ 2

// Bank structure
struct bank {
	int balance[2];
};

// Global bank variable
struct bank *Bank;

// Semaphore global variable
sem_t *mutexPtr;

void* MakeTransactions() {
	int i, j, tmp1, tmp2, rint;
	double dummy; 
	for(i = 0; i < 100; i++) {

		// Force other processes to wait until the semaphore has been signaled
		sem_wait(mutexPtr);
		// Enter the critical section

		// Make a random amount from [0 29] - 15 => [-15 14]
		rint = (rand() % 30) - 15; 
		if(((tmp1 = Bank -> balance[0]) + rint) >= 0 && ((tmp2 = Bank -> balance[1]) - rint) >= 0) {
			// Add a 100 random amounts to A
			Bank -> balance[0] = tmp1 + rint;
			for(j = 0; j < rint * 1000; j++) {
				dummy = 2.345 * 8.765 / 1.234;
			}
			// Subtract the same amount from B
			Bank -> balance[1] = tmp2 - rint;
		}

		// Signal the semaphore
		sem_post(mutexPtr);
		// Exit the critical section

	}
	return NULL;
}

int main(int argc, char **argv) {
	int shmid, shmidSem;
	struct bank *shm;
	pid_t pid;
	srand(getpid());

	// Make a shared memory key for the semaphore
	key_t keySem = ftok("shmfileSem", 64);
	if((shmidSem = shmget(keySem, 1, IPC_CREAT | 0666)) < 0) {
		perror("Error in shmidSem");
		exit(1);
	}
	// If there's an error in attaching to the shared memory segment for the semaphore
	if((mutexPtr = shmat(shmidSem, NULL, 0)) == (sem_t*) - 1) {
		perror("Error in shmatSem");
		exit(1);
	}

	mutexPtr = (sem_t*) (mutexPtr + 1);

	// Initialize the semaphore with sharing
	sem_init(mutexPtr, 1, 1);

	// Make a shared memory key for the bank
	key_t key = ftok("shmfile", 65);
	// If there's an error in creating the shared memory segment for the bank
	if((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
		perror("Error in shmid");
		exit(1);
	}
	// If there's an error in attaching to the shared memory segment for the bank
	if((shm = shmat(shmid, NULL, 0)) == (struct bank*) - 1) {
		perror("Error in shmat");
		exit(1);
	}
	Bank = shm;
	// Start with 100 in each balance
	Bank -> balance[0] = 100;
	Bank -> balance[1] = 100;

	printf("Init balances A:%d + B:%d ==> %d!\n", Bank -> balance[0], Bank -> balance[1], Bank -> balance[0] + Bank -> balance[1]); 

	pid = fork();
	if(pid < 0) {
		perror("Error in fork\n");
		return(1);
	// Child process
	} else if(pid == 0) {
		MakeTransactions();
		exit(0);
	// Parent process
	} else {
		MakeTransactions();
		wait(NULL);
	}

	printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n", Bank -> balance[0], Bank -> balance[1], Bank -> balance[0] + Bank -> balance[1]);
	
	// Destroy the semaphore
	sem_destroy(mutexPtr);

	return 0;
}