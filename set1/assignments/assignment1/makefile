OBJS	= main.o pipes.o circlebuf.o main2.o
OUT	= main, main2

OBJS0	= main.o pipes.o circlebuf.o
SOURCE0	= main.c pipes.c circlebuf.c
HEADER0	= circlebuf.h pipes.h
OUT0	= main

OBJS1	= main2.o pipes.o circlebuf.o
SOURCE1	= main2.c pipes.c circlebuf.c
HEADER1	= circlebuf.h pipes.h
OUT1	= main2

CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: main main2 clean

main: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0) -lpthread

main2: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1) -lpthread

main.o: main.c
	$(CC) $(FLAGS) main.c -lpthread

pipes.o: pipes.c
	$(CC) $(FLAGS) pipes.c -lpthread

circlebuf.o: circlebuf.c
	$(CC) $(FLAGS) circlebuf.c -lpthread

main2.o: main2.c
	$(CC) $(FLAGS) main2.c -lpthread

clean:
	rm -f *.o