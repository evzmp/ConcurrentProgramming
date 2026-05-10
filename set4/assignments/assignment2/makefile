OBJS	= main.o threads.o ../assignment1/crt.o
SOURCE	= main.c threads.c ../assignment1/crt.c
HEADER	= threads.h ../assignment1/crt.h
OUT	= test
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.c
	$(CC) $(FLAGS) main.c 

threads.o: threads.c
	$(CC) $(FLAGS) threads.c 

clean:
	rm -f *.o