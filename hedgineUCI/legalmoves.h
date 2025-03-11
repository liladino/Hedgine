#ifndef LEGALMOVES_H
#define LEGALMOVES_H

//#include "output.h"
#include "structs.h"
#include "moves.h"
#include "slidingmagic.h"

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


bool bitInCheck(const bitboard* const board, bool tomove);

void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlytakes);

resultconst gameend(bitboard board, bool tomove);

#endif
