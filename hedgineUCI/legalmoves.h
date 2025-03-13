#ifndef LEGALMOVES_H
#define LEGALMOVES_H

//#include "output.h"
#include "structs.h"
#include "moves.h"
#include "slidingmagic.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>


//u64 enemypieces(bitboard board, bool enemy);

static __inline__ __attribute__((always_inline)) u64 enemypieces(const bitboard* const board, bool enemy){
	if (enemy == black){ //black
		return (board->piece[bking] | board->piece[bqueen] | board->piece[bbishop] | board->piece[bknight] | board->piece[bpawn] | board->piece[brook]); 
	}
	return (board->piece[wking] | board->piece[wqueen] | board->piece[wbishop] | board->piece[wknight] | board->piece[wpawn] | board->piece[wrook]); 
}

bool bitInCheck(const bitboard* const board, bool tomove);

void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlytakes);

resultconst gameend(bitboard board, bool tomove);

#endif
