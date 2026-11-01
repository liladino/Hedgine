CC = gcc
CFLAGS = -Wall -Wextra -pedantic -MMD
INC_DIR = headers

ifdef DEBUG
CFLAGS += -g -DDEBUG
endif

ifdef BENCH
CFLAGS += -O3 -DBENCH
endif

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)
TARGET = hedgine

# FOR MINGW ON WINDOWS PUT -lws2_32 AT THE END OF THE OBJECT COMPILATION
mingwall: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^ -lws2_32 

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^ 

debug: CFLAGS += -g3 -DDEBUG
debug: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt0: CFLAGS += -O0
opt0: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt1: CFLAGS += -O1
opt1: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt2: CFLAGS += -O2
opt2: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt3: CFLAGS += -O3
opt3: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

opt: CFLAGS += -O3
opt: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^
	
profiling: CFLAGS += -pg
profiling: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

clean:
	rm -f *.o *.d *.out $(FLAGS_STAMP)

# Rebuild everything only when the flags change (e.g. switching between a debug
# and a bench build). The stamp is rewritten only if its content differs, so
# with the same flags make stays incremental. Target specific flags (debug: ...)
# reach this rule too, because they are inherited by the prerequisites.
FLAGS_STAMP = .cflags
$(FLAGS_STAMP): FORCE
	@echo '$(CFLAGS)' | cmp -s - $@ || echo '$(CFLAGS)' > $@
FORCE:
.PHONY: FORCE

%.o: %.c $(FLAGS_STAMP)
	$(CC) $(CFLAGS) -c $< -o $@

release: CFLAGS += -O3
release: $(OBJS)
	$(CC) -Wall -Werror -pedantic -O3 -o $(TARGET) $^
	rm -f *.o
	
-include $(DEPS)
