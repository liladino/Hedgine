CC = gcc
CFLAGS = -Wall -Wextra -pedantic -MMD

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

ifdef OPT
CFLAGS += -O3
endif

ifdef BENCH
CFLAGS += -O3 -DBENCH
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

opt: CFLAGS += -O3
opt: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^
	
profiling: CFLAGS += -pg
profiling: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

clean:
	rm -f *.o *.d

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

release: CFLAGS += -O3
release: $(OBJS)
	$(CC) -Wall -Werror -pedantic -O3 -o $(TARGET) $^
	rm -f *.o
	
-include $(DEPS)
