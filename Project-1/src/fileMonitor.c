#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define SCAN_INTERVAL 5

int main(int argc, char** argv) {
	if (argc <= 3) {
		printf("Wrong number of arguments.\n");
		printf("Usage: monitorFile <numFiles> <filePids> <files>\n");
		exit(-1);
	}

	int numFiles = strtol(argv[1], NULL, 10);
	if (argc != 2 + 2*numFiles) {
		printf("Wrong number of argume	nts: filePids and  files must have the same size.\n");
		printf("Usage: monitorFile <numFiles> <filePids> <files>\n");
		exit(-1);
	}

	// initializing arrays
	int i;
	pid_t filePids[numFiles];
	for (i = 0; i < numFiles; i++)
		filePids[i] = (pid_t) strtol(argv[i+2], NULL, 10);
	char* files[numFiles];
	for (i = 0; i < numFiles; i++)
		files[i] = argv[i+2+numFiles];

	printf("File Monitor has been started.\n");
	int numMonitoredFiles = numFiles;
	while (numMonitoredFiles > 0) {
		for (i = 0; i < numFiles; i++) {
			// skipping if file no longer exists
			if (filePids[i] == 0)
				continue;

			// trying to open file
			if (fopen (files[i],"r") == NULL) {
				// if file could not be opened
				printf("\n%s has been deleted.\n", files[i]);

				// Note:
				// For debugging purposes, -1 is assigned to dummy pids.
				// This is a flag. Use it when you don't want to kill any process.
				if (filePids[i] != -1) {
					printf("Killing the associated monitorAux.\n");
					kill(-filePids[i], SIGUSR1);
				}

				filePids[i] = 0;
				numMonitoredFiles--;
			}

			// checking if there are no more monitored files
			if (numMonitoredFiles == 0)
				break;
		}

		if (numMonitoredFiles != 0)
			sleep(SCAN_INTERVAL);
	}

	printf("File Monitor: Done.\n");

	return 0;
}
