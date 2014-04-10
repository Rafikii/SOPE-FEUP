#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#define TAIL 1
#define GREP 2

int numFiles;
pid_t *pidsPtr;
pid_t pidFileMonitor;

char* getCWDPath() {
	char* cwd = (char*) malloc(1024*sizeof(char));

	if (getcwd(cwd, 1024) == NULL)
		perror("getcwd() error");

	return cwd;
}

char* getMonitorAuxPath() {
	char* cwd = getCWDPath();

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

char* getFileMonitorPath() {
	char* cwd = getCWDPath();

	char* monitorAuxPath;
	if((monitorAuxPath = malloc(strlen(cwd)+1)) != NULL) {
		monitorAuxPath[0] = '\0';	// ensures the memory is an empty string
		strcat(monitorAuxPath, cwd);
		strcat(monitorAuxPath, "/fileMonitor");
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
	kill(pidFileMonitor, SIGINT);
	printf("OK!\n");

	printf("Monitor terminated.\n");
	exit(0);
}

void zombieHandler(int signal_num) {
	// wait for the child process and clean up
	int status;
	wait(&status);
}

void installHandlers() {
	// installing alarm
	struct sigaction act;
	act.sa_handler = alarmHandler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGALRM, &act, NULL);

	// installing zombie handler
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &zombieHandler;

	sigaction(SIGCHLD, &sa, NULL);
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
	pid_t pids[numFiles];
	pidsPtr = pids;

	// initializing pids array
	int i;
	for (i = 0; i < numFiles; i++)
		pids[i] = 0;

	// launching a monitorAux for each file
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

	// launching fileMonitor
	if ((pidFileMonitor = fork()) < 0) {
		printf("Error while forking to create File Monitor.\n");
		exit(3);
	} else if (pidFileMonitor > 0) {
		// parent running
		// do nothing?
	} else {
		// son running

		// making sure every monitorAux has already started
		int done = 0;
		while (!done) {
			done = 1;

			for (i = 0; i < numFiles; i++) {
				if (pids[i] == 0) {
					done = 0;
					break;
				}
			}
		}

		char* str;
		int fileMonitorArgc = 2+2*numFiles;
		char* fileMonitorArgv[fileMonitorArgc+1];

		// setting filename and null terminator
		fileMonitorArgv[0] = "./fileMonitor";
		fileMonitorArgv[fileMonitorArgc] = 0;

		// setting numFiles
		str = (char*) malloc(256*sizeof(char));
		sprintf(str, "%d", numFiles);
		fileMonitorArgv[1] = str;

		// setting filePids and files
		for (i = 0; i < numFiles; i++) {
			str = (char*) malloc(256*sizeof(char));
			sprintf(str, "%d", pids[i]);
			fileMonitorArgv[i+2] = str;

			fileMonitorArgv[i+2+numFiles] = argv[i+3];
		}

		// and only then launch file monitor
		if (execv(getFileMonitorPath(), fileMonitorArgv) != 0) {
			printf("Error trying to execute fileMonitor.\n");
			exit(4);
		}
	}

	installHandlers();

	// setting alarm
	alarm(scanTime);

	// execute monitor while file monitor is running
	while (waitpid(pidFileMonitor, NULL, WNOHANG) != -1);

	printf("Reaping File Monitor.\n");
	kill(pidFileMonitor, SIGINT);

	return 0;
}
