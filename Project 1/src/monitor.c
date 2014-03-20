#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	if (argc <= 3) {
		printf("Wrong number of arguments.\n");
		printf("Usage: monitor <time> <word> <files>\n");
		exit(-1);
	}

	printf("tempo: %s\n", argv[1]);
	printf("palavra: %s\n", argv[2]);

	int i;
	for(i = 3; i < argc; i++) {
		printf("ficheiro %d: %s\n", i-2, argv[i]);
	}

	printf("Done.\n");
	return 0;
}