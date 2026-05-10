OBJS	= train.o
SOURCE	= train.c
HEADER	=
OUT	= train
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: final clean

final: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS) -lpthread

train.o: train.c
	$(CC) $(FLAGS) train.c -lpthread

clean:
	rm -f *.o