#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define MAXLINE 256
#define READ 0
#define WRITE 1

char* getTimeToString() {
	time_t currentTime;

	if ((currentTime = time(NULL)) == (time_t) - 1)
		printf("Error getting current time.");

	char str[20];
	sprintf(str, "%02d-%02d-%02dT%02d:%02d:%02d",
		localtime(&currentTime)->tm_year+1900,
		localtime(&currentTime)->tm_mon+1,
		localtime(&currentTime)->tm_mday,
		localtime(&currentTime)->tm_hour,
		localtime(&currentTime)->tm_min,
		localtime(&currentTime)->tm_sec);

	char* timeStr;
	if((timeStr = malloc(strlen(str)+1)) != NULL){
		timeStr[0] = '\0';	// ensures the memory is an empty string
		strcat(timeStr, str);
	} else {
		printf("malloc failed!\n");
		exit(-1);
	}

	return timeStr;
}

int monitorAux(char* search, char* filename) {
	int n, p1[2], p2[2];
	pid_t pidTail, pidGrep;
	char line[MAXLINE];

	// creating pipes
	if (pipe(p1) < 0 || pipe(p2) < 0) {
		fprintf(stderr, "Error creating pipes. -.-\n");
		exit(1);
	}

	// forking: creating son to run tail
	if ((pidTail = fork()) < 0) {
		fprintf(stderr, "Error while forking to run tail. O.o\n");
		exit(2);
	} else if (pidTail > 0) {
		// forking: creating son to run grep
		if ((pidGrep = fork()) < 0) {
			fprintf(stderr, "Error while forking to run grep. O.o\n");
			exit(3);
		} else if (pidGrep > 0) {
			// parent running
			close(p2[WRITE]);
			//dup2(p2[READ], STDOUT_FILENO);

			while (1) {
				n = read(p2[READ], line, MAXLINE);
				line[n-1] = 0;
				printf("%s - %s - \"%s\"\n", getTimeToString(), filename, line);
			}
		} else {
			// son that runs grep
			close(p1[WRITE]);
			dup2(p1[READ], STDIN_FILENO);

			dup2(p2[WRITE], STDOUT_FILENO);
			close(p2[READ]);

			if (execlp("grep", "grep", "--line-buffered", search, NULL) != 0)
				printf("Error trying to execute grep.\n");
		}
	} else {
		// son that runs tail
		dup2(p1[WRITE], STDOUT_FILENO);
		close(p1[READ]);

		if (execlp("tail", "tail", "-f", "-n", "0", filename, NULL) != 0)
			printf("Error trying to execute tail.\n");
	}

	return 0;
}

void catch_ctrl_c(int signo) {
	char msg[] = "Control - C pressed!\n";
	write(STDERR_FILENO, msg, strlen(msg));
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

	printf("---------------------\n");
	printf("Debugging info:\n");
	printf("tempo: %ld\n", scanTime);
	printf("palavra: %s\n", word);
	printf("---------------------\n");

	time_t startTime = time(NULL);
	time_t currentTime = startTime;
	while (currentTime - startTime < scanTime)
		currentTime = time(NULL);

	pid_t pid;
	int i;
	for (i = 3; i < argc; i++) {
		printf("Starting to monitor file %d: %s\n", i-2, argv[i]);

		if ((pid = fork()) < 0) {
			printf("Error while forking for file: %s\n", argv[i]);
			exit(1);
		} else if (pid > 0) {
			// parent running
			// do nothing?
		} else {
			// son running
			monitorAux(word, argv[i]);

			/*
			if (execlp(monitorAux, monitorAux, word, argv[i], NULL) != 0) {
				printf("Error executing monitorAux for file: %s\n", argv[i]);
				exit(1);
			}
			*/
		}
	}

	struct sigaction act;
	act.sa_handler = catch_ctrl_c;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	
	sigaction(SIGINT, &act, NULL);

	printf("----------\n");
	printf("Time's up!\n");
	printf("----------\n");
	return 0;
}
