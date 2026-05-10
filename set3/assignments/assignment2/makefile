OBJS	= bridge.o
SOURCE	= bridge.c
HEADER	= 
OUT	= bridge
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean


final: $(OBJS)
	$(CC) -g $(OBJS) -o bridge $(LFLAGS) -lpthread

bridge.o: bridge.c
	$(CC) $(FLAGS) bridge.c -lpthread

clean:
	rm -f *.o