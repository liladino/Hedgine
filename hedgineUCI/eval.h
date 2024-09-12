#ifndef EVAL_H
#define EVAL_H

#include "structs.h"
#include "legalmoves.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdbool.h>

int fulleval(const bitboard *board, bool tomove);

#endif
