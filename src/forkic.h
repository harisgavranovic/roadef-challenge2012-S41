#ifndef FORKIC_H_
#define FORKIC_H_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "testovi.h"
#include "parameters.h"


int solveA(Parameters& param1, Solution* sol1, Parameters& param2, Solution* sol2)
{



	int64* data;
	key_t key;
	int shmid;

	// Initialize Shared Memory
	key = ftok("thread1.c",'R');
	shmid = shmget(key, 1024, 0644 | IPC_CREAT);

	// Attach to Shared Memory
	data = shmat(shmid, (void *)0, 0);
	if(data == (int64 *)(-1)) perror("shmat");

	// Write initial value to shared memory
	*data = 100000000000000;

	param1.best_objective = data;
	param2.best_objective = data;

	sol1->setParams(param1);
	sol2->setParams(param2);

   pid_t  pid;
   pid = fork();

   if (pid == -1)
   {
      fprintf(stderr, "can't fork, error %d\n", errno);
      exit(EXIT_FAILURE);
   }

   if (pid == 0)
   {

      solveAFinal(param1, sol1);
      _exit(0);

   }
   else
   {
      solveAFinal(param2, sol2);
      return 0;
   }

   return 0;

}

int solveB(Parameters& param1, Solution* sol1, Parameters& param2, Solution* sol2)
{


	int64* data;
	key_t key;
	int shmid;

	// Initialize Shared Memory
	key = ftok("thread1.c",'R');
	shmid = shmget(key, 1024, 0644 | IPC_CREAT);

	// Attach to Shared Memory
	data = shmat(shmid, (void *)0, 0);
	if(data == (int64 *)(-1)) perror("shmat");

	// Write initial value to shared memory
	*data = 100000000000000;

	param1.best_objective = data;
	param2.best_objective = data;

	sol1->setParams(param1);
	sol2->setParams(param2);

	*(param1.best_objective) = 100000000000000; *(param2.best_objective) = 100000000000000;

	   pid_t  pid;
	   pid = fork();

	   if (pid == -1)
	   {
	      fprintf(stderr, "can't fork, error %d\n", errno);
	      exit(EXIT_FAILURE);
	   }

	   if (pid == 0)
	   {

	      solveBFinal(param1, sol1);
	      _exit(0);

	   }
	   else
	   {
	      solveBFinal2(param2, sol2);
	      return 0;
	   }

	   return 0;

}

#endif /* FORKIC_H_ */
