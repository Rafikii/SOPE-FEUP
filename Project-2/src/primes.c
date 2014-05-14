#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "CircularQueue.h"

#define DEBUG_MODE 0
#define QUEUE_SIZE 10

int writeID;
int* primesList;
int primesListSize;

// TODO can this be global?
long n;
sem_t termSem;

int compare(const void * a, const void * b) {
	return *(int*) a - *(int*) b;
}

void addNumToPrimesList(int num) {
	primesList[writeID++] = num;
	primesListSize++;

	if (DEBUG_MODE)
		printf("Added %d to primes list.\n", num);
}

void printPrimesList() {
	int i;
	for (i = 0; i < primesListSize; i++)
		printf("%d ", primesList[i]);
	printf("\n");
}

void* filterThread(void* arg) {
	CircularQueue* inputCircularQueue = arg;

	QueueElem num = queue_get(inputCircularQueue);
	if (num > sqrt(n)) {
		do {
			// adding num to primes list
			addNumToPrimesList(num);

			// reading next element from the circular queue
			num = queue_get(inputCircularQueue);
		} while (num != 0);

		// signaling semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: num > sqrt(n)\n");
		sem_post(&termSem);
	} else {
		int temp = num;

		// creating output circular queue
		CircularQueue* outputCircularQueue;
		queue_init(&outputCircularQueue, QUEUE_SIZE);

		// creating filter thread
		pthread_t ft;
		pthread_create(&ft, NULL, filterThread, outputCircularQueue);

		do {
			// reading next element from the circular queue
			num = queue_get(inputCircularQueue);

			if (num % temp != 0 || num == 0)
				queue_put(outputCircularQueue, num);
		} while (num != 0);

		// adding first number to primes list
		addNumToPrimesList(temp);
	}

	return NULL ;
}

void* initThreadFunc(void* arg) {
	printf("> Starting initial thread\n");

	addNumToPrimesList(2);

	if (n > 2) {
		// creating output circular queue
		CircularQueue* outputCircularQueue;
		queue_init(&outputCircularQueue, QUEUE_SIZE);

		// creating filter thread
		pthread_t ft;
		pthread_create(&ft, NULL, filterThread, outputCircularQueue);

		// placing odd numbers in the output circular queue
		int i;
		for (i = 3; i <= n; i += 2) {
			queue_put(outputCircularQueue, i);
		}

		// placing a 0 at the end of the queue to terminate the sequence
		queue_put(outputCircularQueue, 0);
	} else {
		// signaling semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: n = 2\n");
		sem_post(&termSem);
	}

	return NULL ;
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
	n = strtol(argv[1], &pEnd, 10);

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

	// initializing termination semaphore
	if (sem_init(&termSem, 0, 0) != 0)
		return -1;

	// starting initial thread
	pthread_t initThread;
	pthread_create(&initThread, NULL, initThreadFunc, NULL );
	pthread_join(initThread, NULL );

	// waiting for semaphore
	sem_wait(&termSem);

	// sorting primes list
	qsort(primesList, primesListSize, sizeof(int), compare);

	printf("\n");
	printf("The list of primes between 2 and %ld is:\n", n);
	printPrimesList();
	printf("\n");

	printf("Program: Done!\n");
	return 0;
}
