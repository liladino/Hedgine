#ifndef PGNOUTPUT_H
#define PGNOUTPUT_H

//#include "engine.h"
#include "feninput.h"
#include "fenoutput.h"
//#include "game.h"
//#include "input.h"
#include "legalmoves.h"
#include "moves.h"
//#include "output.h"
//#include "pgninput.h"

#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void toSANconvertKnight(char board[12][12], move m, int *f, int *r);

void toSANconvertBishop(char board[12][12], move m, int *f, int *r);

void toSANconvertRook(char board[12][12], move m, int *f, int *r);

void toSANconvertQueen(char board[12][12], move m, int *f, int *r);

void disambiguation(char board[12][12], move m, char *SAN, int *i);

void printmeta(FILE *fptr, int playercol);

void convert_back(char board[12][12], move m, char *SAN);

int savepgn(char *FEN, movelist *game, int playercol, int result);

#endif
