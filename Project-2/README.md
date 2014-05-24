#SOPE - FEUP

#####Project summary:  
A program to generate a list of prime numbers based on the [sieve of Eratosthenes](http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes).  
Implemented using a circular queue, threads, semaphores, mutexes and condition variables.

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

Using [this](https://github.com/Rafikii/SOPE-FEUP/commit/cff5fc12cda00fd6a779fbaab5f7e5b89fe43ce9) commit.  
Using the commands:
- Simple mode: ``` time ./bin/primes n 0 0 ```
- Using condVar: ``` time ./bin/primes n 0 1 ```

#####n = 100
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.004s      | 0.004s	    	|
| 2       | 0.005s      | 0.004s	    	|
| 3       | 0.004s		  | 0.004s	    	|
| Mean    | 0.004s	    | 0.004s	    	|

#####n = 1 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.007s      | 0.008s	    	|
| 2       | 0.008s      | 0.007s	    	|
| 3       | 0.008s      | 0.007s	    	|
| Mean    | 0.008s      | 0.007s	    	|

#####n = 10 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.042s      | 0.040s	    	|
| 2       | 0.044s      | 0.043s	    	|
| 3       | 0.042s      | 0.038s	    	|
| Mean    | 0.043s      | 0.040s	    	|

#####n = 100 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 0.485s      | 0.506s	    	|
| 2       | 0.496s      | 0.485s	    	|
| 3       | 0.498s      | 0.489s	    	|
| Mean    | 0.493s      | 0.493s	    	|

#####n = 1 000 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 8.267s      | 8.297s	    	|
| 2       | 8.401s      | 8.230s	    	|
| 3       | 8.232s      | 8.220s	    	|
| Mean    | 8.300s      | 8.249s	    	|

#####n = 10 000 000
| Run no. | Simple mode | Using condVar |
| ------: | :---------- | :------------ |
| 1       | 3m08.341s   | 3m07.204s	   	|
| 2       | 3m17.966s   | 3m11.508s	   	|
| 3       | 3m17.896s   | 3m12.805s	   	|
| Mean    | 3m14.734s   | 3m10.506s	   	|

