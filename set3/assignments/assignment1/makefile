OBJS	= primes.o
SOURCE	= primes.c
HEADER	= 
OUT	= primes
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -lpthread

primes.o: primes.c
	$(CC) $(FLAGS) primes.c -lpthread

clean:
	rm -f *.o