#SOPE - FEUP

#####Project summary:  
A program to generate a list of prime numbers based on the [sieve of Eratosthenes](http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes).  
Implemented using a [circular queue](/Project-2/src/CircularQueue.h), threads, semaphores, mutexes and condition variables.

Authors:
- Henrique Ferrolho					
- Rafaela Faria

Professor:
- Jorge Silva

MIEIC - 2014

***
Speed Tests
-----------

####24th, May - 2014

Using [this](https://github.com/Rafikii/SOPE-FEUP/commit/2cb0774fde8d7c20f7a4ef885fb0d121a8c1c5dd) commit.  
Using the commands:
- Simple mode: ```time ./bin/primes n 0 0 0```
- Using condVar: ```time ./bin/primes n 0 0 1```

#####n = 100
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.004s      | 0.004s	    	|
| 2       | 0.004s      | 0.003s	    	|
| 3       | 0.003s		  | 0.003s	    	|
| Mean    | 0.004s	    | 0.003s	    	|

#####n = 1 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.007s      | 0.006s	    	|
| 2       | 0.006s      | 0.006s	    	|
| 3       | 0.006s      | 0.006s	    	|
| Mean    | 0.006s      | 0.006s	    	|

#####n = 10 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.014s      | 0.013s	    	|
| 2       | 0.018s      | 0.016s	    	|
| 3       | 0.017s      | 0.014s	    	|
| Mean    | 0.016s      | 0.014s	    	|

#####n = 100 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.120s      | 0.126s	    	|
| 2       | 0.124s      | 0.110s	    	|
| 3       | 0.112s      | 0.118s	    	|
| Mean    | 0.119s      | 0.118s	    	|

#####n = 1 000 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 2.743s      | 2.740s	    	|
| 2       | 2.520s      | 2.948s	    	|
| 3       | 2.865s      | 2.039s	    	|
| Mean    | 2.709s      | 2.576s	    	|

#####n = 10 000 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 1m35.666s   | 1m32.679s	   	|
| 2       | 1m35.383s   | 1m33.688s	   	|
| 3       | 1m35.610s   | 1m32.595s	   	|
| Mean    | 1m35.553s   | 1m32.987s	   	|
