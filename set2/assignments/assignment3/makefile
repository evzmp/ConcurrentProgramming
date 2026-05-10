OBJS	= mysem.o bridge.o
SOURCE	= ../assignment1/mysem.c bridge.c
HEADER	= ../assignment1/mysem.h
OUT	= bridge
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean


final: $(OBJS)
	$(CC) -g $(OBJS) -o bridge $(LFLAGS) -lpthread

mysem.o: ../assignment1/mysem.c
	$(CC) $(FLAGS) ../assignment1/mysem.c  -lpthread

bridge.o: bridge.c
	$(CC) $(FLAGS) bridge.c -lpthread

clean:
	rm -f *.o