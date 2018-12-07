#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMTASKS 25000
#define NANOSECONDS_PER_SECOND 1E9
#define FIBNUM 26

int fib(int x) {
  if (x == 0) 
    return 0;
  else if (x == 1) 
    return 1;
  return fib(x - 1) + fib(x - 2);
}

void * workerthread(void * tid){
  fib(FIBNUM);
  return NULL;
}

int main(){
  pthread_t thr[NUMTASKS];
  struct timespec before, after;
  
  printf("running fib(%d) %d times\n",FIBNUM,NUMTASKS);
  clock_gettime(CLOCK_REALTIME, &before);
  for (int i = 0; i < NUMTASKS; i++) {
    int rcode;
    if ((rcode = pthread_create(&thr[i], NULL, workerthread, NULL))) {
      if (rcode == EAGAIN){
        // insufficient resources, just keep slamming it until we finish our work
        i--;
        continue;
      }
      fprintf(stderr, "Error in pthread_create at thread num %d: %d\n", i,rcode);
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < NUMTASKS; i++) {
    pthread_join(thr[i], NULL);
  }
  clock_gettime(CLOCK_REALTIME,&after);
  long difference = ((after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec) - (before.tv_sec * NANOSECONDS_PER_SECOND + before.tv_nsec)) / NUMTASKS;
  printf("Thread per task time: %ld nanoseconds.\n",difference);
  return 0;
}
