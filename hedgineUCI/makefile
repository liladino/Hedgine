CC = gcc
CFLAGS = -Wall -Wextra -pedantic -MMD

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
DEPS = $(OBJS:.o=.d)
TARGET = hedgine

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

debug: CFLAGS += -g -DDEBUG
debug: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt: CFLAGS += -O2
opt: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

clean:
	rm -f *.o *.d

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

release: CFLAGS += -O2
release: $(OBJS)
	$(CC) -Wall -Werror -pedantic -O2 -o $(TARGET) $^
	rm -f *.o
	
-include $(DEPS)
