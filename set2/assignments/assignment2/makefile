OBJS	= mysem.o primes.o
SOURCE	= ../assignment1/mysem.c primes.c
HEADER	= ../assignment1/mysem.h
OUT	= primes
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -lpthread

mysem.o: ../assignment1/mysem.c
	$(CC) $(FLAGS) ../assignment1/mysem.c  -lpthread

primes.o: primes.c
	$(CC) $(FLAGS) primes.c -lpthread

clean:
	rm -f *.o