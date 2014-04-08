#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define TAIL 1
#define GREP 2

int numFiles;
pid_t (*pidsPtr);

char* getMonitorAuxPath() {
	// getting current work directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		perror("getcwd() error");

	char* monitorAuxPath;
	if((monitorAuxPath = malloc(strlen(cwd)+1)) != NULL) {
		monitorAuxPath[0] = '\0';	// ensures the memory is an empty string
		strcat(monitorAuxPath, cwd);
		strcat(monitorAuxPath, "/monitorAux");
	} else {
		printf("malloc failed!\n");
		exit(-1);
	}

	return monitorAuxPath;
}

void alarmHandler(int signum) {
	printf("\nAlarm went off!\n");

	printf("Killing all processes... ");
	int i, j;
	for (i = 0; i < numFiles; i++) {
		for (j = 0; j < 3; j++) {
			kill(-pidsPtr[i], SIGINT);
		}
	}
	printf("OK!\n");

	printf("Monitor terminated.\n");
	exit(0);
}

int main(int argc, char** argv) {
	if (argc <= 3) {
		printf("Wrong number of arguments.\n");
		printf("Usage: monitor <time> <word> <files>\n");
		exit(-1);
	}

	// processing time and word input
	long scanTime = strtol(argv[1], NULL, 10);
	char* word = argv[2];
	numFiles = argc-3;

	// Debugging block
	printf("---------------------\n");
	printf("Monitor info:\n");
	printf("Scan time: %ld seconds\n", scanTime);
	printf("Word: %s\n", word);
	printf("---------------------\n");

	// creating array to store pids
	pid_t pids[argc-3];
	pidsPtr = pids;

	int i;
	for (i = 0; i < numFiles; i++) {
		if ((pids[i] = fork()) < 0) {
			printf("Error while forking for file: %s\n", argv[i+3]);
			exit(1);
		} else if (pids[i] > 0) {
			// parent running
			// do nothing?
		} else {
			// son running
			if (execlp(getMonitorAuxPath(), "monitorAux", word, argv[i+3], NULL) != 0) {
				printf("Error trying to execute monitorAux.\n");
				exit(2);
			}
		}
	}

	struct sigaction act;
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGALRM, &act, NULL);

	// setting alarm
	alarm(scanTime);

	while (1);

	return 0;
}
