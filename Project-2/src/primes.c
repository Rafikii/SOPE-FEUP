#include <stdio.h>
#include <stdlib.h>
/*
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
*/

#include "CircularQueue.h"

#define QUEUE_SIZE 10

int main(int argc, char** argv) {
	CircularQueue *q;
	queue_init(&q, QUEUE_SIZE);

	// validating number of arguments
	if (argc <= 1) {
		printf("Wrong number of arguments.\n");
		printf("Usage: primes <N>\n");
		return -1;
	}

	// processing input
	char* pEnd;
	long n = strtol(argv[1], &pEnd, 10);

	// validating input
	if (argv[1] == pEnd || n < 2) {
		printf("Invalid argument.\n");
		printf("<N> must be an integer greater or equal than 2.\n");
		return -1;
	}

	// Debugging block
	printf("------------\n");
	printf("Primes info:\n");
	printf("N: %ld\n", n);
	printf("------------\n");

	printf("Done!\n");
	return 0;
}
