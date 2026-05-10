OBJS	= crt.o test.o
SOURCE	= crt.c test.c
HEADER	= crt.h
OUT	= test
CC	 = gcc
FLAGS	 = -g -c -Wall -std=c99
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

crt.o: crt.c
	$(CC) $(FLAGS) crt.c 

test.o: test.c
	$(CC) $(FLAGS) test.c 

clean:
	rm -f *.o