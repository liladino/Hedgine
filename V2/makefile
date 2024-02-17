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
	$(CC) $(CFLAGS) -c bitlegalmoves.c bitmoves.c bitengine.c random.c bithash.c

release: CFLAGS += -O2
release: $(OBJS)
	$(CC) -Wall -pedantic -Werror -O2 -o clchess $^
	rm -f *.o
