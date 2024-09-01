#ifndef LEGALMOVES_H
#define LEGALMOVES_H

//#include "output.h"
#include "structs.h"
#include "moves.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif
#ifdef BENCH
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>


//u64 enemypieces(bitboard board, bool enemy);


bool bitInCheck(bitboard board, bool tomove);

void bitGenerateLegalmoves(move_array* moves, bitboard board, bool tomove, u64 *attackedsquares, bool onlytakes);

resultconst gameend(bitboard board, bool tomove);

#endif
