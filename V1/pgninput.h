#ifndef PGNINPUT_H
#define PGNINPUT_H

//#include "engine.h"
#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"
#include "input.h"
#include "legalmoves.h"
#include "moves.h"
#include "output.h"

//#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *readPGN(char *filename);

int makegame(char PGN[], movelist *game, char board[12][12], bool *tomove, int *halfmove, int *movenum, int *fmv, int castling[4], squarenums *enpass);

int loadPGN(char *filename, char board[12][12], movelist *game, bool *tomove, int castling[], squarenums *enpass, int *halfmove, int *movenum, int *fmv);

#endif
