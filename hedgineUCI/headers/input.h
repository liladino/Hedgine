#ifndef INPUT_H
#define INPUT_H

#include "moves.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int readLineDynamic(char** str, const size_t maxSize);

move parseLongAlgebraicNotation(char str[]);

int setboardFEN(char FEN[], bitboard* bboard, bool *tomove, int *fmv, int *movenum);

#endif
