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

all: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

debug: CFLAGS += -g -DDEBUG
debug: $(OBJS)
	$(CC) $(CFLAGS) -o all $^

opt: CFLAGS += -O2
opt: $(OBJS)
	$(CC) $(CFLAGS) -o all $^

clean:
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

bits:
	$(CC) $(CFLAGS) -c bitlegalmoves.c bitmoves.c bitengine.c bithash.c random.c

release: CFLAGS += -O2
release: $(OBJS)
	$(CC) -Wall -Werror -pedantic -o clchess $^
	rm -f *.o
