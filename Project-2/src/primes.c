//------------------------------------------------------------------------------------------
// SOPE - MIEIC
// 2014 - FEUP
//
// Project summary:
// A program to generate a list of prime numbers based on the sieve of Eratosthenes.
// Implemented using a circular queue, threads, semaphores, mutexes and condition variables.
//
// Authors:								Professor:
// Henrique Ferrolho					Jorge Silva
// Rafaela Faria
//------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "CircularQueue.h"

//------------------------------------------------------------------------------------------
// Usage options variables and their default values
int DEBUG_MODE = 0;
int QUEUE_SIZE = 10;
int THREAD_TERMINATION_MODE = 0;

//------------------------------------------------------------------------------------------
// Global variables to store the list of primes
int writeIndex;
unsigned long* primesList;

//------------------------------------------------------------------------------------------
// Program global variables
long n;
sem_t termSem;
pthread_mutex_t primesListAccessControlMutex;

//------------------------------------------------------------------------------------------
// Global variables to use the condition variable
int runningThreadsCount;
pthread_cond_t condVar;
pthread_mutex_t condVarMutex;

//------------------------------------------------------------------------------------------
// Functions used to sort the final list of primes
int compare(const void* a, const void* b) {
	return *(unsigned long*) a - *(unsigned long*) b;
}

//------------------------------------------------------------------------------------------
// Functions to manipulate runningThreadsCount variable
void changeRunningThreadsCount(int value) {
	pthread_mutex_lock(&condVarMutex);

	runningThreadsCount += value;
	if (DEBUG_MODE)
		printf("Number of running threads: %d\n", runningThreadsCount);

	pthread_cond_signal(&condVar);
	pthread_mutex_unlock(&condVarMutex);
}

void incRunningThreadsCount() {
	if (DEBUG_MODE)
		printf("Incrementing running threads counter.\n");

	changeRunningThreadsCount(1);
}

void decRunningThreadsCount() {
	if (DEBUG_MODE)
		printf("Decrementing running threads counter.\n");

	changeRunningThreadsCount(-1);
}

//------------------------------------------------------------------------------------------
// Adds number 'num' to the primes list safely using 'primesListAccessControlMutex'
void addNumToPrimesList(int num) {
	if (pthread_mutex_lock(&primesListAccessControlMutex) != 0)
		fprintf(stderr, "Error: Failed when locking primes list access control mutex: %s\n", strerror(errno));

	primesList[writeIndex++] = num;

	if (DEBUG_MODE)
		printf("Added %d to primes list.\n", num);

	if (pthread_mutex_unlock(&primesListAccessControlMutex) != 0)
		fprintf(stderr, "Error: Failed when locking primes list access control mutex: %s\n", strerror(errno));
}

//------------------------------------------------------------------------------------------
// Prints the list of primes
void printPrimesList() {
	int i;
	for (i = 0; i < writeIndex; i++)
		printf("%ld ", primesList[i]);
	printf("\n");
}

//------------------------------------------------------------------------------------------
// Filter thread function:
// Receives the address of a circular queue which contains the numbers to filter.
void* filterThread(void* arg) {
	if (DEBUG_MODE)
		printf("> Starting a filter thread\n");

	// first of all, update 'runningThreadsCounter'
	incRunningThreadsCount();

	// get the input circular queue from the argument received
	CircularQueue* inputCircularQueue = (CircularQueue*) arg;

	// read the 'head' of the input circular queue
	QueueElem num = queue_get(inputCircularQueue);

	// if the 'head' is greater than the square root of 'n'
	if (num > sqrt(n)) {
		// add all the input queue elements to the list of primes
		do {
			// add num to primes list
			addNumToPrimesList(num);

			// read next element from the input circular queue
			num = queue_get(inputCircularQueue);
		} while (num != 0);

		// update 'runningThreadsCounter'
		decRunningThreadsCount();

		// wait for all the threads to terminate by using 'condVar'
		pthread_mutex_lock(&condVarMutex);
		// this is NOT a busy waiting
		while (runningThreadsCount != 0)
			pthread_cond_wait(&condVar, &condVarMutex);
		pthread_mutex_unlock(&condVarMutex);

		// signal termination semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: num > sqrt(n)\n");
		if (sem_post(&termSem) != 0)
			fprintf(stderr, "Error: Failed when posting termination semaphore: %s\n", strerror(errno));
	} else {
		// temporarily save the 'head' of the circular queue
		int temp = num;

		// create output circular queue
		CircularQueue* outputCircularQueue;
		if (queue_init(&outputCircularQueue, QUEUE_SIZE) != 0) {
			fprintf(stderr, "Error: Failed to initialize output circular queue.\n");
			return NULL;
		}

		// create another filter thread
		pthread_t ft;
		if (pthread_create(&ft, NULL, filterThread, outputCircularQueue) != 0) {
			fprintf(stderr, "Error: Failed to create filter thread: %s\n", strerror(errno));
			return NULL;
		}

		// read all the input circular queue elems until '0' is read
		do {
			// read element from the circular queue
			num = queue_get(inputCircularQueue);

			// put read element in the output circular queue if
			// it is not a multiple of temp or it is equal to 0
			if (num % temp != 0 || num == 0)
				queue_put(outputCircularQueue, num);
		} while (num != 0);

		// add the temporarily saved 'head' of the input queue to the primes list
		addNumToPrimesList(temp);

		// update 'runningThreadsCounter'
		decRunningThreadsCount();
	}

	// destroy the input circular queue
	queue_destroy(inputCircularQueue);

	return NULL;
}

//------------------------------------------------------------------------------------------
// Initial thread function:
void* initThreadFunc(void* arg) {
	if (DEBUG_MODE)
		printf("> Starting initial thread\n");

	// add number 2 to the primes list
	addNumToPrimesList(2);

	// if 'n' is greater than 2
	if (n > 2) {
		// create output circular queue
		CircularQueue* outputCircularQueue;
		if (queue_init(&outputCircularQueue, QUEUE_SIZE) != 0) {
			fprintf(stderr, "Error: Failed to initialize output circular queue.\n");
			return NULL;
		}

		// create a filter thread
		pthread_t ft;
		if (pthread_create(&ft, NULL, filterThread, outputCircularQueue) != 0) {
			fprintf(stderr, "Error: Failed to create filter thread: %s\n", strerror(errno));
			return NULL;
		}

		// place all the odd numbers less than or equal to 'n' in the output circular queue
		int i;
		for (i = 3; i <= n; i += 2)
			queue_put(outputCircularQueue, i);

		// place '0' at the end of the queue to terminate the sequence
		queue_put(outputCircularQueue, 0);
	} else {
		// if 'n' is equal to 2, there is no need to calculate any more prime numbers
		// signal the termination semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: n = 2\n");
		if (sem_post(&termSem) != 0)
			fprintf(stderr, "Error: Failed when posting termination semaphore: %s\n", strerror(errno));
	}

	return NULL ;
}

//------------------------------------------------------------------------------------------
// Function to validate and process the arguments received
int processAndValidateArguments(int argc, char** argv) {
	// validate the number of arguments
	if (argc <= 1 || argc > 5) {
		printf("\n");
		printf("Wrong number of arguments.\n");
		printf("Usage: primes <n>\n");
		printf("Usage: primes <n> <debug mode>\n");
		printf("Usage: primes <n> <debug mode> <queue size> \n");
		printf("Usage: primes <n> <debug mode> <queue size> <thread termination mode>\n");
		printf("\n");

		return -1;
	}

	// pointer to assure valid convertion from string to long
	char* pEnd;

	// process and validate <debug mode> paramater
	if (argc > 2) {
		DEBUG_MODE = strtol(argv[2], &pEnd, 10);
		if (argv[2] == pEnd || (DEBUG_MODE != 0 && DEBUG_MODE != 1)) {
			printf("\n");
			printf("Invalid argument.\n");
			printf("<debug mode> must be 0 or 1.\n");
			printf("\n");

			return -1;
		}

		if (DEBUG_MODE)
			printf("DEBUG_MODE value overrided to: %d\n", DEBUG_MODE);
	}

	if (DEBUG_MODE)
		printf("> Processing and validating received arguments.\n");

	// process and validate <queue size> paramater
	if (argc > 3) {
		QUEUE_SIZE = strtol(argv[3], &pEnd, 10);
		if (argv[3] == pEnd || QUEUE_SIZE <= 0) {
			printf("\n");
			printf("Invalid argument.\n");
			printf("<queue size> must be an integer greater than 0.\n");
			printf("\n");

			return -1;
		}

		if (DEBUG_MODE)
			printf("QUEUE_SIZE value overrided to: %d\n", QUEUE_SIZE);
	}

	// process and validate <thread termination mode> paramater
	if (argc > 4) {
		THREAD_TERMINATION_MODE = strtol(argv[4], &pEnd, 10);
		if (argv[4] == pEnd || (THREAD_TERMINATION_MODE != 0 && THREAD_TERMINATION_MODE != 1)) {
			printf("\n");
			printf("Invalid argument.\n");
			printf("<thread termination mode> must be 0 (simple mode) or 1 (use condition variable).\n");
			printf("\n");

			return -1;
		}

		if (DEBUG_MODE)
			printf("THREAD_TERMINATION_MODE value overrided to: %d\n", THREAD_TERMINATION_MODE);
	}

	// process and validate <n> paramater
	n = strtol(argv[1], &pEnd, 10);
	if (argv[1] == pEnd || n < 2) {
		printf("\n");
		printf("Invalid argument.\n");
		printf("<n> must be an integer greater than or equal to 2.\n");
		printf("\n");

		return -1;
	}

	// if user does not specify queue size
	if (argc < 4) {
		// override it to n/2 for maximum performance
		QUEUE_SIZE = n / 2;

		if (DEBUG_MODE)
			printf("Setting QUEUE_SIZE to n/2 for maximum performance.\n");
	}

	return 0;
}

//------------------------------------------------------------------------------------------
// Function to correctly initialize all required program variables
int initializeProgramData() {
	if (DEBUG_MODE)
		printf("> Initializing program data.\n");

	// initialize "shared" data
	writeIndex = 0;
	int allocationSize = ceil(1.2 * n / log(n)) + 1;
	primesList = (unsigned long*) malloc(allocationSize * sizeof(unsigned long));
	if (primesList == NULL) {
		fprintf(stderr, "Error: Failed to allocate space for the primes list: %s\n", strerror(errno));
		return -1;
	}

	// initialize termination semaphore
	if (sem_init(&termSem, 0, 0) != 0) {
		fprintf(stderr, "Error: Failed to initialize termination semaphore: %s\n", strerror(errno));
		return -1;
	}

	// initialize primes list access control mutex
	if (pthread_mutex_init(&primesListAccessControlMutex, NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize primes list access control mutex: %s\n", strerror(errno));
		return -1;
	}

	// initialize condition variable, condition variable mutex and 'runningThreadsCount'
	runningThreadsCount = 0;
	if (pthread_cond_init(&condVar, NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize condition variable: %s\n", strerror(errno));
		return -1;
	}
	if (pthread_mutex_init(&condVarMutex, NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize condition variable mutex: %s\n", strerror(errno));
		return -1;
	}

	if (DEBUG_MODE) {
		// displaying received data info
		printf("---------------\n");
		printf("Primes info:\n");
		printf("n: %ld\n", n);
		printf("queue size: %d\n", QUEUE_SIZE);
		printf("debug mode: %d\n", DEBUG_MODE);
		printf("---------------\n");
	}

	return 0;
}

//------------------------------------------------------------------------------------------
// Function to free/destroy allocated memory
int freeProgramMemory() {
	if (DEBUG_MODE)
		printf("> Freeing program memory.\n");

	// free the list of prime numbers
	free(primesList);

	// destroy 'termSem'
	if (sem_destroy(&termSem) != 0) {
		fprintf(stderr, "Error: Failed to destroy termination semaphore: %s\n", strerror(errno));
		return -1;
	}

	// destroy 'primesListAccessControlMutex'
	if (pthread_mutex_destroy(&primesListAccessControlMutex) != 0) {
		fprintf(stderr, "Error: Failed to destroy primes list access control mutex: %s\n", strerror(errno));
		return -1;
	}

	// destroy 'condVar'
	if (pthread_cond_destroy(&condVar) != 0) {
		fprintf(stderr, "Error: Failed to destroy the condition variable: %s\n", strerror(errno));
		return -1;
	}

	// destroy 'condVarMutex'
	if (pthread_mutex_destroy(&condVarMutex) != 0) {
		fprintf(stderr, "Error: Failed to destroy the condition variable mutex: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

//------------------------------------------------------------------------------------------
// Main function
int main(int argc, char** argv) {
	// process and validate all arguments received
	if (processAndValidateArguments(argc, argv) != 0) {
		fprintf(stderr, "Error: Failed to process received arguments.\n");
		return -1;
	}

	// initialize data
	if (initializeProgramData() != 0) {
		fprintf(stderr, "Error: Failed to initialize program data.\n");
		return -1;
	}

	// start initial thread
	pthread_t initThread;
	if (pthread_create(&initThread, NULL, initThreadFunc, NULL) != 0) {
		fprintf(stderr, "Error: Failed to create initial thread: %s\n", strerror(errno));
		return -1;
	}

	// wait for termination semaphore
	if (sem_wait(&termSem) != 0)
		fprintf(stderr, "Error: Failed when waiting for termination semaphore: %s\n", strerror(errno));

	// sort primes list
	if (DEBUG_MODE)
		printf("Sorting primes list\n");
	qsort(primesList, writeIndex, sizeof(unsigned long), compare);

	// finally display the list of prime numbers
	printf("\n");
	printf("The list of prime numbers between 2 and %ld is:\n", n);
	printPrimesList();
	printf("\n");

	// free memory
	if (freeProgramMemory() != 0) {
		fprintf(stderr, "Error: Failed to free program memory.\n");
		return -1;
	}

	if (DEBUG_MODE)
		printf("Program Terminated.\n");
	return 0;
}
