/*=========================================================*/ 
/* race.c --- for playing with ECE437 */ 
/*=========================================================*/ 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h> // Added this to use the kill function
#define SHMSZ 2

struct bigbank{
  int balance[2];
  } Bank; 

sem_t *mtx; 
struct bigbank *d;

void* MakeTransactions() { //routine for thread execution 
	int i, j, tmp1, tmp2, rint; 
	double dummy; 
	sem_wait(mtx);

	for (i=0; i < 100; i++) { 
	rint = (rand()%30)-15; 
	if (((tmp1=d->balance[0])+rint)>=0 && ((tmp2=d->balance[1])-rint)>=0) { 
		d->balance[0] = tmp1 + rint; 
		for (j=0; j < rint*1000; j++) {
	 		dummy=1.546*5.545/4.543;
	 		dummy=2.345*8.765/1.234;
			} // spend time on purpose 
			d->balance[1] = tmp2 - rint; 
			}
			sem_post(mtx);	
		}	 
 return NULL; 
}

void CreateSemSpace(){
  int shmid2;

  key_t key2 = ftok("Shmfile2",87);

  // Create semaphore memory space
  if ((shmid2 = shmget(key2, 32, IPC_CREAT | 0666)) < 0){
	printf("Error in shmget2\n");
	exit(1);
}
  // Attach it to our object 
  if ((mtx = shmat(shmid2,NULL,0)) == (sem_t*) -1){
	printf("Error in shmat2\n");
	exit(1);
}

  mtx = (sem_t*) (mtx + 1); 

}

void CreateMemSpace(){
  int shmid1;
  key_t key1 = ftok("Shmfile1",12);

 // Create memory space segment 
 if ((shmid1 = shmget(key1, 32, IPC_CREAT | 0666)) < 0){
	printf("Error in shmget\n");
	exit(1);
}
 // Attach it to our object
 if ((d = shmat(shmid1, NULL,0)) == (struct bigbank*)-1){
	printf("Error in shmat\n");
	exit(1);
}
 // Setting up the balance 
 d->balance[0] = 100;
 d->balance[1] = 100;

}

int main(int argc, char **argv) {
	srand(time(0));
	
	// Creating a shared memory segment for the bank variable
	CreateMemSpace();
	// Creating a memory segment for the semaphore
	CreateSemSpace();

	sem_init(mtx,1,1);

	for(int j=0;j<80;j++){
	printf("Init balances A:%d + B:%d ==> %d\n", d->balance[0],d->balance[1],d->balance[0]+d->balance[1]);

	pid_t pid = fork();
	if (pid == 0){    // This is the Child Process 
		MakeTransactions();
		exit(0);
	}
	else if(pid < 0){ // Error in case the child failed to create
		printf("Failed to create child");
		return 1;
	}
	else{             // This is the Parent Process
		MakeTransactions(); 
		wait(NULL);
	}
	 printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n",d->balance[0],d->balance[1],d->balance[0]+d->balance[1]); 
	
	}
sem_destroy(mtx);
 return 0;
}
