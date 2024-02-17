#ifndef FENINPUT_H
#define FENINPUT_H


//#include "engine.h"

//#include "fenoutput.h"
//#include "game.h"
//#include "input.h"
#include "legalmoves.h"
#include "moves.h"
//#include "output.h"
//#include "pgninput.h"
//#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int setboardFEN(char FEN[], char board[12][12], bool *tomove, int castling[], squarenums *enpass, int *fmv, int *movenum);

char *readFEN(char *filename);

int loadFEN(char *filename, char board[12][12], bool *tomove, int castling[], squarenums *enpass, int *fmv, int *movenum);

#endif
