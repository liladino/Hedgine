CC = gcc
CFLAGS = -Wall -Werror -O2

SRC = engine.c feninput.c fenoutput.c game.c input.c legalmoves.c main.c moves.c output.c pgninput.c pgnoutput.c
TARGET = clchess

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clchess: engine.o feninput.o fenoutput.o game.o input.o legalmoves.o main.o moves.o output.o pgninput.o pgnoutput.o
	$(CC) $(CFLAGS) engine.o feninput.o fenoutput.o game.o input.o legalmoves.o main.o moves.o output.o pgninput.o pgnoutput.o -o $(TARGET)
	rm *.o
