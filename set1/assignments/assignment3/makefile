OBJS	= main.o merge.o
SOURCE	= main.c merge.c
HEADER	= merge.h
OUT	= main
CC	 = gcc
FLAGS	 = -g -c -Wall

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lpthread

main.o: main.c
	$(CC) $(FLAGS) main.c -lpthread

merge.o: merge.c
	$(CC) $(FLAGS) merge.c -lpthread

clean:
	rm -f *.o