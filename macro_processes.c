#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int main(){
  int wpid, status, retval;
  struct timespec before;
  struct timespec after;
  
  clock_gettime(CLOCK_REALTIME,&before);
  for (int i = 0; i < NUMTASKS; i++) {
    retval = fork();
    if (retval == -1){
      // over our process limit: wait for at least one process to finish and try again
      wpid = wait(&status);
      if (wpid < 0){
        printf("waited when no children to wait for\n");
        exit(1);
      }
      // try this iteration of the loop over again
      i--;
      continue;
    }
    if (retval == 0){
      fib(FIBNUM);
      exit(0);
    }
    else{
      continue;
    }
  }
  // wait for all worker processes to finish
  while ((wpid = wait(&status)) > 0);
  clock_gettime(CLOCK_REALTIME,&after);
  long difference = ((after.tv_sec * NANOSECONDS_PER_SECOND + after.tv_nsec) - (before.tv_sec * NANOSECONDS_PER_SECOND + before.tv_nsec)) / NUMTASKS;
  printf("Process per task time: %ld nanoseconds.\n",difference);
  
  return 0;
}
