# ParallelPrimeFinder
This project is parallel implementation of a prime number finder algorithm for CMPE478 (Parallel Processing) course at Boğaziçi University.
Implementation of the algorithm is prime.cpp and it is implemented using OpenMP

To compile code file, you should open a shell on a UNIX system and go to these files' directory and call make command 

example of the use:
./prime M Chunksize (--printnumbers)
M is upper bound and Chunksize is size of a chunk which is used in parallel region.
--printnumbers is an option to print prime numbers to the console
