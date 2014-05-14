#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
/*
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
*/

#include "CircularQueue.h"

#define QUEUE_SIZE 10

int writeID;
int* primesList;
int primesListSize;

CircularQueue* sharedQueue;

int compare (const void * a, const void * b) {
	return *(int*)a - *(int*)b;
}

void printPrimesList() {
	int i;
	for (i = 0; i < primesListSize; i++)
		printf("%d ", primesList[i]);
	printf("\n");
}

void addNumToPrimesList(int num) {
	primesList[writeID++] = num;
	primesListSize++;
}

void* filterThread(void* arg) {
	int elem = 0;

	while (elem != 0) {
		printf("%d", elem);

		elem = 1;
	}
	printf("\n");

	return NULL;
}

void* initThreadFunc(void* arg) {
	printf("> Starting initial thread\n");

	addNumToPrimesList(2);

	pthread_t ft;
	pthread_create(&ft, NULL, filterThread, NULL);

	queue_init(&sharedQueue, QUEUE_SIZE);

	int i;
	for (i = 0; i <= 10; i++)
		queue_put(sharedQueue, i);

	return NULL;
}

int main(int argc, char** argv) {
	// validating number of arguments
	if (argc <= 1) {
		printf("Wrong number of arguments.\n");
		printf("Usage: primes <n>\n");
		return -1;
	}

	// processing input
	char* pEnd;
	long n = strtol(argv[1], &pEnd, 10);

	// validating input
	if (argv[1] == pEnd || n < 2) {
		printf("Invalid argument.\n");
		printf("<n> must be an integer greater or equal than 2.\n");
		return -1;
	}

	// Debugging block
	printf("------------\n");
	printf("Primes info:\n");
	printf("n: %ld\n", n);
	printf("------------\n");

	// initializing shared data
	writeID = 0;
	int allocationSize = 1.2 * n / log(n);
	primesList = (int*) malloc(allocationSize * sizeof(int));
	primesListSize = 0;

	// starting initial thread
	pthread_t initThread;
	pthread_create(&initThread, NULL, initThreadFunc, argv[1]);
	pthread_join(initThread,NULL);

	// sorting primes list
	qsort(primesList, primesListSize, sizeof(int), compare);

	printf("\n");
	printf("The list of primes between 2 and %ld is:\n", n);
	printPrimesList();
	printf("\n");

	printf("Program: Done!\n");
	return 0;
}
