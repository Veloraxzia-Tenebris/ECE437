/*=========================================================*/ 
/* race.c --- for playing with ECE437 */ 
/*=========================================================*/ 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h> // Added this to use the kill function
#define SHMSZ 2

struct bigbank{int balance[2];} Bank={{100,100}}; //global variable defined 

struct bigbank *d;
void* MakeTransactions() { //routine for thread execution 
	int i, j, tmp1, tmp2, rint; double dummy; 
	for (i=0; i < 100; i++) { 
	rint = (rand()%30)-15; 
	if (((tmp1=d->balance[0])+rint)>=0 && ((tmp2=d->balance[1])-rint)>=0) { 
	d->balance[0] = tmp1 + rint; 
	for (j=0; j < rint*1000; j++) {
	 dummy=1.546*5.545/4.543;
	 dummy=2.345*8.765/1.234;} // spend time on purpose 
	d->balance[1] = tmp2 - rint; 
} 
} 
 return NULL; 
}

int main(int argc, char **argv) {
	srand(time(0));
	int i;
	int shmid;
	char *shm,*s;

	//void* voidptr=NULL;  

	key_t key = ftok("shmfile",65);
	if ((shmid = shmget(key,SHMSZ,IPC_CREAT | 0666))<0){
		printf("Error in shmid");
		return 1;
	}
	if ((d = shmat(shmid, NULL,0))==(struct bigbank *)-1){
		printf("error in shmat");
		return 1;
	}

	d->balance[1] = 100;
	d->balance[0] = 100;

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
//		wait(NULL);
	}
	 printf("Let's check the balances A:%d + B:%d ==> %d ?= 200\n",d->balance[0],d->balance[1],d->balance[0]+d->balance[1]); 
//	 kill(pid,SIGKILL);
	}
 return 0;
}
