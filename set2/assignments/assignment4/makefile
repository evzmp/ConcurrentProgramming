OBJS	= mysem.o train.o
SOURCE	= ../assignment1/mysem.c train.c
HEADER	= ../assignment1/mysem.h
OUT	= train
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -lpthread

mysem.o: ../assignment1/mysem.c
	$(CC) $(FLAGS) ../assignment1/mysem.c  -lpthread

train.o: train.c
	$(CC) $(FLAGS) train.c -lpthread

clean:
	rm -f *.o