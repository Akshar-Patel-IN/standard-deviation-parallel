#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int  argc, char *argv[]){

	int i;
	long *num,numOfElems;
	double sum,mean,variance,stdDev,elapsTime;
	clock_t start,end;

	//command-line arguments
	if(argc < 2){
		printf("Too few arguments\n");
		exit(1);
	} else {
		numOfElems = atol(argv[1]);
	}

	//allocate memory
	num = malloc(numOfElems * sizeof(long));

	//random numbers input
	srand(time(NULL));
	for(i = 0; i < numOfElems; i++) {
		num[i] = i;
	}

	//get start time
	start = clock();

	//calculate mean
	sum = 0;
	for(i = 0; i < numOfElems; i++) {
		sum = sum + num[i];
	}
	mean = sum / numOfElems;

	//calculate standard deviation
	sum = 0;
	for(i = 0; i < numOfElems; i++) {
		sum = sum + (mean - num[i]) * (mean - num[i]);
	}

	variance = (double) sum / numOfElems;
	stdDev = sqrt(variance);
	printf("\nStandard Deviation = %lf",stdDev);

	//get end time
	end = clock();

	//calculate total time
	elapsTime = (double)(end - start) / CLOCKS_PER_SEC;
	printf("\n\nTotal Time(in Milliseconds) = %lf\n\n",elapsTime * 1000);

	return 0;
}
