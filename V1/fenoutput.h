#ifndef FENOUTPUT_H
#define FENOUTPUT_H

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
#include "engine.h"
#include "feninput.h"

void setfilename(char *filename, char *extension);

int savefen(char board[12][12], bool tomove, int castling[], squarenums enpass, int fmv, int movenum);

#endif
