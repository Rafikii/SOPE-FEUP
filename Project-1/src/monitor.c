#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int monitorAux(char* search, char* filename) {
	int p1[2], p2[2];
	pid_t pidTail, pidGrep;

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
		} else {
			// son that runs grep
			close(p1[WRITE]);
			close(p2[READ]);
			dup2(p1[READ], STDIN_FILENO);

			if (execlp("grep", "grep", "--line-buffered", search, NULL) != 0)
				printf("Error trying to execute grep.\n");
		}
	} else {
		// son that runs tail
		close(p1[READ]);
		dup2(p1[WRITE], STDOUT_FILENO);

		if (execlp("tail", "tail", "-f", "-n", "0", filename, NULL) != 0)
			printf("Error trying to execute tail.\n");
	}

	return 0;
}

int main(int argc, char** argv) {
	if (argc <= 3) {
		printf("Wrong number of arguments.\n");
		printf("Usage: monitor <time> <word> <files>\n");
		exit(-1);
	}

	char* word = argv[2];

	printf("tempo: %s\n", argv[1]);
	printf("palavra: %s\n", word);

	int i;
	for (i = 3; i < argc; i++) {
		printf("Starting to monitor file %d: %s\n", i-2, argv[i]);
		monitorAux(word, argv[i]);
	}

	printf("Done.\n");
	return 0;
}
