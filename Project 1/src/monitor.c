#include <stdio.h>

int main(int argc, char** argv) {
	printf("tempo: %s\n", argv[1]);
	printf("palavra: %s\n", argv[2]);

	int i;
	for(i = 3; i < argc; i++) {
		printf("ficheiro %d: %s\n", i-2, argv[i]);
	}

	printf("Done.\n");
	return 0;
}