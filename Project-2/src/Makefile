all: primes

primes: primes.o CircularQueue.o
	gcc -Wall primes.o CircularQueue.o -o bin/primes -lm -pthread

primes.o: primes.c
	gcc -Wall -c primes.c

CircularQueue.o: CircularQueue.c
	gcc -Wall -c CircularQueue.c

clean:
	rm -rf *o bin/primes
