CC = gcc
CFLAGS = -Wall -pedantic

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

ifdef OPT
CFLAGS += -O2
endif

ifdef BENCH
CFLAGS += -O2 -DBENCH
endif

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

bits:
	$(CC) $(CFLAGS) -c bitlegalmoves.c bitmoves.c bitengine.c

bitstest:
	$(CC) $(CFLAGS) -c bitlegalmoves.c bitengine.c bitmoves.c output.c feninput.c moves.c
	$(CC) $(CFLAGS) bitlegalmoves.o bitengine.o bitmoves.o output.o feninput.o moves.o -o test
	
