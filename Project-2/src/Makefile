all: primes
	mkdir -p bin
	mv primes bin/
	mv *o bin/

primes: primes.o CircularQueue.o
	gcc -Wall primes.o CircularQueue.o -o primes -lm -pthread

primes.o: primes.c
	gcc -Wall -c primes.c

CircularQueue.o: CircularQueue.c
	gcc -Wall -c CircularQueue.c

clean:
	rm -rf bin/
