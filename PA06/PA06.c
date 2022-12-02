#include "random437.h" // If gcc breaks make sure to add -lm flag 
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <math.h>

#define MAXWAITPEOPLE 800 // Maximum people that can be in the line before being rejected 

pthread_mutex_t shared_mutex;

// struct for our clock
typedef struct time{
  int sec;
  int mins;
  int hours;
}Time;


int MAXPERCAR,CARNUM;      // maximum people per car, max cars, and current people in line for ride
int RUNTIME = 600;         // Time in virtual seconds for the ride to run for 
int TotalPeopleShowed = 0;  // Total people that showed up
int Rejected = 0;          // counter for people who were rejected 
int PeopleWaiting = 0;     // ammout of people currently in line
int TimeWaiting = 0;       // Ammount of time people were waiting 
int LongestLine = 0;       // Line at its longest
int WorstTime = 0;         // Worst time in the park 
int count = 0;             // counter for steps 
int TotalPeople = 0;       // total people that arrived at the park 
Time t = {0,0,9};          // Park opens at 0 sec 0 mins 9 hours or 9AM
Time wct;                  // Worst case for time 

// Convert the current time in seconds into its hours mins and seconds format 
void timer(int seconds){
  t.sec += seconds;
  if (t.sec == 60){
    t.sec = 0;
    ++t.mins;
  }
  if (t.mins == 60){
    t.mins = 0;
    ++t.hours;
  }
  if (t.hours == 24){
    t.hours == 0;
  }
}

// Used to call the poisson function based on the correct averages from HW page
int mean_arrivales(Time m){
  // check to see if time is withen a acceptable range
  if ((m.hours < 0) || (m.hours > 23) || (m.mins < 0) || (m.mins > 59) || (m.sec < 0) || (m.sec >59))
  {
    printf("invalid time was entered\n");
    exit(1);
  }
  else{
    switch(m.hours)
        {
        case 9:
          return 25;

        case 10:
          return 25;

        case 11: 
          return 45;

        case 12:
          return 45;

        case 13:
          return 45;

        case 14:
          return 35;

        case 15:
          return 25;

        case 16:
          return 25;
          
        case 17:
          return 25;

        case 18:
          return 25;

        default:
          printf("Ride is closed");
          return(0);
          break;

        } 
      }
}

// calculate ammount of people that arrived at any given minute
int arrivals(Time n){
  int mean = mean_arrivales(n);
  return poissonRandom(mean);
}

// Thread to load the passengers into the cars 
void *LoadPassengers(void *varp){
  pthread_mutex_lock(&shared_mutex);
  if(PeopleWaiting >= MAXPERCAR){
    // pick up a load of people
    PeopleWaiting -= MAXPERCAR;
  }
  else{
    // Took the last bit of people that were waiting 
    PeopleWaiting = 0;
  }
  pthread_mutex_unlock(&shared_mutex);
}

// Thread to take the passengers from the line and put them on the car
void *RideOperator(void *varp){
  // take or reject people incoming

  pthread_mutex_lock(&shared_mutex); 
  int arrived = arrivals(t);
  int turned = 0; // ammount of people turned away at that time
  TotalPeopleShowed += arrived;

  if(MAXWAITPEOPLE >= (arrived+PeopleWaiting)){
    PeopleWaiting += arrived;
  }
  else{
    turned = ((arrived + PeopleWaiting) - MAXWAITPEOPLE); 
    Rejected += turned;
    PeopleWaiting = MAXWAITPEOPLE;
  }
  if (PeopleWaiting > (CARNUM*MAXPERCAR)){
    int tpw = PeopleWaiting;
      while((int)ceil(tpw/(CARNUM*MAXPERCAR))){
      // stuff to calculate time waiting 
       int WaitTime = (ceil(tpw/(CARNUM*MAXPERCAR))-1);
       TimeWaiting += (WaitTime*(CARNUM*MAXPERCAR));
       tpw -= (CARNUM*MAXPERCAR);
      }
  }
  // update worst time if we have a longer line added >= so if there is 800 people again the worst time updates
  if(PeopleWaiting >= LongestLine){
    LongestLine = PeopleWaiting;
    wct.hours = t.hours;
    wct.mins = t.mins;
    wct.sec = t.sec;
  }
  printf("%d, arrived %d, Rejected %d, In line %d, at %d:%d:%d\n",count ,arrived, turned, PeopleWaiting, t.hours, t.mins, t.sec);
  printf("\n");
  pthread_mutex_unlock(&shared_mutex);
}


int main(int argc, char *argv[]){

// option handler stuff
int option;
  if(argc < 5){
    printf("Usage: ./PA06 -N 'CARNUM' -M 'MAXPERCAR'\n");
    exit(2);
  }
  while((option = getopt(argc,argv,"N:M:"))!=-1){
  	switch (option){

      case 'N':
		  CARNUM = atoi(optarg);
		  printf("N was %s\n", optarg);
		  break;

		  case 'M':
		  MAXPERCAR = atoi(optarg);
		  printf("M was %s\n", optarg);
		  break;

		  default:
		  printf("incorrect options specified, Exiting!\n");
		  exit(1);
                      } 
						}
                              
// Main for loop to run the test for a set ammout of seconds 600 or 10 Hours
  int k = 0;
  while(k < RUNTIME){
    pthread_t judge,t[CARNUM];
    pthread_mutex_init(&shared_mutex, NULL);
    // create thread for the ride operator
    if(pthread_create(&judge, NULL, RideOperator, NULL) != 0)
      return 1;
    if(pthread_join(judge,NULL) != 0)
      return 2;
    // Create thread for the cars taking passengers
    for(int i = 0; i < CARNUM; i++)
      if(pthread_create(&t[i],NULL,LoadPassengers,NULL)!=0)
        return 1;
    for(int i = 0; i < CARNUM; i++)
      if(pthread_join(t[i],NULL)!=0)
        return 2;
      timer(7);  // for loading the people
      timer(53); // time for ride to complete
      pthread_mutex_destroy(&shared_mutex); // Become what we sought to destroy
      k++;
      count++;   // update count value for step 
    }

int TotalRide = TotalPeopleShowed - Rejected; // Calculate ammount of people who rode ride
Rejected += PeopleWaiting; // Add people who were in line at close to rejected
int AverageWaitTime = TimeWaiting/TotalRide;  // Calculate average wait time (MAY BE BROKEN)

printf("Total attendance:%d\n", TotalPeopleShowed);
printf("Total Number that went on ride:%d\n",TotalRide);
printf("Total people rejected:%d\n", Rejected);
printf("Average Waiting time:%d Minutes\n", AverageWaitTime);
printf("Longest line was %d people at %d:%d:%d\n", LongestLine, wct.hours, wct.mins, wct.sec);
return 0;
}
