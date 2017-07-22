#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "lib.h"

int main(int argc, char *argv[]) {
	
	int pid,numOfProc,i,*lock,*bar,finalSumMeanId,
	finalSumVarianceId, lockId, barId;
	long *num,numOfElems,blockSize,blockStart,blockEnd,
	remElem,partSumMean,*finalSumMean;
	double partSumVariance,*finalSumVariance,mean,elapsTime;
	clock_t start,end;

	//command-line arguments
	if(argc < 2){
		printf("Too few arguments\n");
		exit(1);
	} else {
		numOfProc = atoi(argv[1]);
		numOfElems = atol(argv[2]);
	}

	//allocate memory
	num = malloc(numOfElems * sizeof(long));

	//random numbers input
	srand(time(NULL));
	for(i = 0; i < numOfElems; i++) {
		num[i] = rand();
	}

	//get start time
	start = clock();

	//get shared memory
	finalSumMean = sshared(sizeof(long), &finalSumMeanId);
	finalSumVariance = sshared(sizeof(double), &finalSumVarianceId);
	lock = sshared(sizeof(int), &lockId);
	bar = sshared(sizeof(int), &barId);

	//initialize lock and barrier
	spin_lock_init(lock);
	barrier_init(bar, numOfProc);

	//calculate block size and remaning elements
	blockSize = numOfElems / numOfProc;
	remElem = numOfElems % numOfProc;

	//fork process
	pid = process_fork(numOfProc);

	//calculate block start index and adjust remaning elements
	blockStart = (pid) * blockSize;
	if(remElem != 0 && pid > 0) blockStart += pid;

	//calucate sum
	partSumMean = 0;
	for(int j = blockStart; j < blockStart + blockSize + 1 && j < numOfElems; j++) {
		partSumMean = partSumMean + num[j];
	}
	spin_lock(lock);
	*finalSumMean = *finalSumMean + partSumMean;
	spin_unlock(lock);

	//create barrier
	barrier(bar);

	//calculate mean
	mean = (float)*finalSumMean / numOfElems;

	//calculate variance sum
	partSumVariance = 0;
	for(int j = blockStart; j < blockStart + blockSize + 1 && j < numOfElems; j++) {
		partSumVariance = partSumVariance + (mean - num[j]) * (mean - num[j]);
	}

	spin_lock(lock);
	*finalSumVariance = *finalSumVariance + partSumVariance;
	spin_unlock(lock);

	//join process
	process_join(numOfProc, pid);

	//calculate standard deviation
	printf("\nStandard Deviation: %f", sqrt(*finalSumVariance / numOfElems));

	//get end time
	end = clock();

	//calculate total time
	elapsTime = (double)(end - start) / CLOCKS_PER_SEC;
	printf("\n\nTotal Time(in Milliseconds) = %lf\n\n",elapsTime * 1000);

	//clean memory
	cleanup_memory(&finalSumMeanId);
	cleanup_memory(&finalSumVarianceId);
	cleanup_memory(&lockId);
	cleanup_memory(&barId);

	return 0;

}
