#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "CircularQueue.h"

int DEBUG_MODE = 0;
int QUEUE_SIZE = 10;

int writeIndex;
unsigned long* primesList;

long n;
sem_t termSem;
pthread_mutex_t primesListAccessControlMutex, mut;
int threadCount;
pthread_cond_t var;

void incThreadCount() {
	pthread_mutex_lock(&mut);
	threadCount++;
	//printf("threadCount: %d\n", threadCount);
	pthread_cond_signal(&var);
	pthread_mutex_unlock(&mut);
}

void decThreadCount() {
	pthread_mutex_lock(&mut);
	threadCount--;
	//printf("threadCount: %d\n", threadCount);
	pthread_cond_signal(&var);
	pthread_mutex_unlock(&mut);
}

int compare(const void * a, const void * b) {
	return *(unsigned long*) a - *(unsigned long*) b;
}

void addNumToPrimesList(int num) {
	if (pthread_mutex_lock(&primesListAccessControlMutex) != 0)
		fprintf(stderr, "Error: Failed when locking primes list access control mutex: %s\n", strerror(errno));

	primesList[writeIndex++] = num;

	if (DEBUG_MODE)
		printf("Added %d to primes list.\n", num);

	if (pthread_mutex_unlock(&primesListAccessControlMutex) != 0)
		fprintf(stderr, "Error: Failed when locking primes list access control mutex: %s\n", strerror(errno));
}

void printPrimesList() {
	int i;
	for (i = 0; i < writeIndex; i++)
		printf("%ld ", primesList[i]);
	printf("\n");
}

void* filterThread(void* arg) {
	incThreadCount();

	CircularQueue* inputCircularQueue = arg;

	QueueElem num = queue_get(inputCircularQueue);
	if (num > sqrt(n)) {
		do {
			// adding num to primes list
			addNumToPrimesList(num);

			// reading next element from the circular queue
			num = queue_get(inputCircularQueue);
		} while (num != 0);

		decThreadCount();

		// waiting for all threads to terminate
		pthread_mutex_lock(&mut);
		while (threadCount != 0)
			pthread_cond_wait(&var, &mut);
		pthread_mutex_unlock(&mut);

		// signaling termination semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: num > sqrt(n)\n");
		if (sem_post(&termSem) != 0)
			fprintf(stderr, "Error: Failed when posting termination semaphore: %s\n", strerror(errno));
	} else {
		int temp = num;

		// creating output circular queue
		CircularQueue* outputCircularQueue;
		if (queue_init(&outputCircularQueue, QUEUE_SIZE) != 0) {
			fprintf(stderr, "Error: Failed to initialize output circular queue.\n");
			return NULL;
		}

		// creating filter thread
		pthread_t ft;
		if (pthread_create(&ft, NULL, filterThread, outputCircularQueue) != 0) {
			fprintf(stderr, "Error: Failed to create filter thread: %s\n", strerror(errno));
			return NULL;
		}

		do {
			// reading next element from the circular queue
			num = queue_get(inputCircularQueue);

			// outputing element to circular queue if it
			// is not a multiple of temp or it is equal to 0
			if (num % temp != 0 || num == 0)
				queue_put(outputCircularQueue, num);
		} while (num != 0);

		// adding first number to primes list
		addNumToPrimesList(temp);
	}

	// destroying input circular queue
	queue_destroy(inputCircularQueue);

	decThreadCount();

	return NULL;
}

void* initThreadFunc(void* arg) {
	if (DEBUG_MODE)
		printf("> Starting initial thread\n");

	// adding number 2 to primes list
	addNumToPrimesList(2);

	if (n > 2) {
		// creating output circular queue
		CircularQueue* outputCircularQueue;
		if (queue_init(&outputCircularQueue, QUEUE_SIZE) != 0) {
			fprintf(stderr, "Error: Failed to initialize output circular queue.\n");
			return NULL;
		}

		// creating filter thread
		pthread_t ft;
		if (pthread_create(&ft, NULL, filterThread, outputCircularQueue) != 0) {
			fprintf(stderr, "Error: Failed to create filter thread: %s\n", strerror(errno));
			return NULL;
		}

		// placing odd numbers in the output circular queue
		int i;
		for (i = 3; i <= n; i += 2)
			queue_put(outputCircularQueue, i);

		// placing zero at the end of the queue to terminate the sequence
		queue_put(outputCircularQueue, 0);
	} else {
		// signaling termination semaphore
		if (DEBUG_MODE)
			printf("Signaling semaphore: n = 2\n");
		if (sem_post(&termSem) != 0)
			fprintf(stderr, "Error: Failed when posting termination semaphore: %s\n", strerror(errno));
	}

	return NULL ;
}

int main(int argc, char** argv) {
	// validating number of arguments
	if (argc <= 1 || argc > 4) {
		printf("\n");
		printf("Wrong number of arguments.\n");
		printf("Usage: primes <n>\n");
		printf("Usage: primes <n> <queue size>\n");
		printf("Usage: primes <n> <queue size> <debug mode>\n");
		printf("\n");

		return -1;
	}

	char* pEnd;

	// processing and validating <debug mode> paramater
	if (argc == 4) {
		DEBUG_MODE = strtol(argv[3], &pEnd, 10);
		if (argv[3] == pEnd || (DEBUG_MODE != 0 && DEBUG_MODE != 1)) {
			printf("\n");
			printf("Invalid argument.\n");
			printf("<debug mode> must be 0 or 1.\n");
			printf("\n");

			return -1;
		}

		if (DEBUG_MODE)
			printf("DEBUG_MODE value overrided to: %d\n", DEBUG_MODE);
	}

	// processing and validating <queue size> paramater
	if (argc >= 3) {
		QUEUE_SIZE = strtol(argv[2], &pEnd, 10);
		if (argv[2] == pEnd || QUEUE_SIZE <= 0) {
			printf("\n");
			printf("Invalid argument.\n");
			printf("<queue size> must be an integer greater than 0.\n");
			printf("\n");

			return -1;
		}

		if (DEBUG_MODE)
			printf("QUEUE_SIZE value overrided to: %d\n", QUEUE_SIZE);
	}

	// processing and validating <n> paramater
	n = strtol(argv[1], &pEnd, 10);
	if (argv[1] == pEnd || n < 2) {
		printf("\n");
		printf("Invalid argument.\n");
		printf("<n> must be an integer greater than or equal to 2.\n");
		printf("\n");

		return -1;
	}

	// if user does not specify queue size, use <n> for maximum performance
	if (argc == 2) {
		QUEUE_SIZE = n / 2;

		if (DEBUG_MODE)
			printf("Setting QUEUE_SIZE to <n> for maximum performance.\n");
	}

	// displaying received data info
	if (DEBUG_MODE) {
		printf("---------------\n");
		printf("Primes info:\n");
		printf("n: %ld\n", n);
		printf("queue size: %d\n", QUEUE_SIZE);
		printf("debug mode: %d\n", DEBUG_MODE);
		printf("---------------\n");
	}

	// initializing shared data
	writeIndex = 0;
	int allocationSize = ceil(1.2 * n / log(n)) + 1;
	primesList = (unsigned long*) malloc(allocationSize * sizeof(unsigned long));
	if (primesList == NULL) {
		fprintf(stderr, "Error: Failed to allocate space for the primes list: %s\n", strerror(errno));
		return -1;
	}

	// initializing termination semaphore
	if (sem_init(&termSem, 0, 0) != 0) {
		fprintf(stderr, "Error: Failed to initialize termination semaphore: %s\n", strerror(errno));
		return -1;
	}

	// initializing primes list access control mutex
	if (pthread_mutex_init(&primesListAccessControlMutex, NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize primes list access control mutex: %s\n", strerror(errno));
		return -1;
	}

	// initializing mut
	threadCount = 0;
	if (pthread_mutex_init(&mut, NULL) != 0) {
		fprintf(stderr, "Error: Failed to initialize primes list access control mutex: %s\n", strerror(errno));
		return -1;
	}
	pthread_cond_init(&var, NULL);

	// starting initial thread
	pthread_t initThread;
	if (pthread_create(&initThread, NULL, initThreadFunc, NULL) != 0) {
		fprintf(stderr, "Error: Failed to create initial thread: %s\n", strerror(errno));
		return -1;
	}

	// waiting for termination semaphore
	if (sem_wait(&termSem) != 0)
		fprintf(stderr, "Error: Failed when waiting for termination semaphore: %s\n", strerror(errno));

	// sorting primes list
	if (DEBUG_MODE)
		printf("Sorting primes list\n");
	qsort(primesList, writeIndex, sizeof(unsigned long), compare);

	// displaying primes list
	printf("\n");
	printf("The list of primes between 2 and %ld is:\n", n);
	printPrimesList();
	printf("\n");

	// freeing memory
	sem_destroy(&termSem);
	pthread_mutex_destroy(&primesListAccessControlMutex);
	free(primesList);

	if (DEBUG_MODE)
		printf("Program Terminated.\n");
	return 0;
}
