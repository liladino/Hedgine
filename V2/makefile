CC = gcc
CFLAGS = -Wall -Wextra -pedantic

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
TARGET = clc

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

debug: CFLAGS += -g -DDEBUG
debug: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt: CFLAGS += -O2
opt: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

clean:
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

bits:
	$(CC) $(CFLAGS) -c bitlegalmoves.c bitmoves.c bitengine.c bithash.c mtwister.c

release: 
	make opt
	rm -f *.o
