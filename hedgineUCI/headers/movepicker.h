#ifndef MOVEPICKER_H
#define MOVEPICKER_H

//#include "output.h"
#include "structs.h"
#include "legalmoves.h"
#include "moves.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>

void initMovePicker(
    MovePicker *picker,
    const bitMove *hint,
    const bitMove *killer0,
    const bitMove *killer1,
    const int history[12][64],
    bool inCheck,
    bool quiescence);

bool nextMove(MovePicker *picker, const bitboard *board, bool tomove, bitMove *out);

bool sameMove(const bitMove* const a, const bitMove* const b);

#endif
