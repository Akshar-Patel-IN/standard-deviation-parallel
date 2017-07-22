#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

int numOfThrs;
pthread_mutex_t mutex;
pthread_barrier_t barrier;
long numOfElems, finalSumMean = 0,*num;
double finalSumVariance = 0.0;

void *thrFun(void *arg){

  int thrNo,i;
  long blockSize,blockStart,blockEnd,remElem,partSumMean;
  double partSumVariance,mean;

  //get thread number
  thrNo = *((int *) arg);

  //calculate block size and remaing elements
  blockSize = numOfElems / numOfThrs;
  remElem = numOfElems % numOfThrs;

  //calculate block start index and adjust remaning elements
  blockStart = (thrNo) * blockSize;
  if(remElem != 0 && thrNo > 0) blockStart += thrNo;

  //calucate sum
  partSumMean = 0;
  for(int j = blockStart;
	j < blockStart + blockSize + 1 && j < numOfElems; j++) {
    partSumMean = partSumMean + num[j];
  }

  pthread_mutex_lock (&mutex);
  finalSumMean = finalSumMean + partSumMean;
  pthread_mutex_unlock (&mutex);

  //barrier
  pthread_barrier_wait(&barrier);

  //calculate mean
  mean = (float)finalSumMean / numOfElems;

  //calculate variance sum
  partSumVariance = 0;
  for(int j = blockStart;
	j < blockStart + blockSize + 1 && j < numOfElems; j++) {
    partSumVariance = partSumVariance
					+ (mean - num[j]) * (mean - num[j]);
  }

  pthread_mutex_lock (&mutex);
  finalSumVariance = finalSumVariance + partSumVariance;
  pthread_mutex_unlock (&mutex);

  pthread_exit(NULL);

}


int main(int argc, char *argv[]){

  pthread_t *threads;
  int *args,i;
  clock_t start,end;
  double elapsTime;

  //command-line arguments
  if(argc < 2){
    printf("Too few arguments\n");
    exit(1);
  } else {
    numOfThrs = atoi(argv[1]);
    numOfElems = atol(argv[2]);
  }

  //allocate memory
  num = malloc(numOfElems * sizeof(long));
  threads = malloc(numOfThrs * sizeof(pthread_t));
  args = malloc(numOfThrs * sizeof(int));

  //random integers input
  srand(time(NULL));
  for(i = 0; i < numOfElems; i++) {
    num[i] = rand();
  }

  //get start time
  start = clock();

  //intitalize lock and barrier
  pthread_mutex_init(&mutex, NULL);
  pthread_barrier_init(&barrier, NULL, numOfThrs);

  //create threads
  for(i = 0;i < numOfThrs;i++){
    args[i] = i;
    int res = pthread_create(&threads[i], NULL, thrFun, &args[i]);
    if(res != 0){
      printf("Error while creating threads.\n");
      exit(1);
    }
  }

  //join threads
  for(i = 0;i < numOfThrs;i++){
    int res = pthread_join(threads[i], NULL);
    if(res != 0){
      printf("Error while joining threads.\n");
      exit(1);
    }
  }

  //get end time
  end = clock();

  printf("\nStandard Deviation: %lf",
	sqrt(finalSumVariance / numOfElems));

  //calculate total time
  elapsTime = (double)(end - start) / CLOCKS_PER_SEC;
  printf("\n\nTotal Time(in Milliseconds) = %lf\n\n",
	elapsTime * 1000);

  //free memory
  free(num);
  free(threads);
  free(args);

  return 0;
}
