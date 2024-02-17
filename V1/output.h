#ifndef OUTPUT_H
#define OUTPUT_H

//#include "engine.h"
//#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"
//#include "input.h"
//#include "legalmoves.h"
//#include "moves.h"

//#include "pgninput.h"
//#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdbool.h>

void printboard_letters(char board[12][12]);

void printblackandwhite(char board[12][12], int view, bool tomove);

void printboard(char board[12][12], int view, int col, bool tomove);

#endif
