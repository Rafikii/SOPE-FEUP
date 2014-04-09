#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char** argv) {
	printf("File Monitor has been started.\n");
	int n = strtol(argv[1], NULL, 10);
	pid_t* pidsCopy = (pid_t*) strtol(argv[2], NULL, 10);
	char** files = (char**) strtol(argv[3], NULL, 10);
	pid_t pidMonitor = strtol(argv[4], NULL, 10);

	int monitoredFilesNum = n;
	while (monitoredFilesNum > 0) {
		int i;
		for (i = 0; i < n; i++) {
			// skipping if file no longer exists
			if (pidsCopy[i] == 0)
				continue;

			// trying to open file
			if (fopen (files[i],"r") == NULL) {
				// if file could not be opened
				printf("\n%s has been deleted.\n", files[i]);
				printf("Killing the associated monitorAux.\n");
				kill(-pidsCopy[i], SIGUSR1);
				pidsCopy[i] = 0;
				monitoredFilesNum--;
			}

			// checking if there are no more monitored files
			if (monitoredFilesNum == 0)
				break;
		}

		if (monitoredFilesNum != 0)
			sleep(5);
	}

	printf("Killing monitor.\n");
	kill(pidMonitor, SIGINT);

	return 0;
}
