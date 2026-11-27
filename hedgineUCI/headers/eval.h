#ifndef EVAL_H
#define EVAL_H

#include "structs.h"
#include "legalmoves.h"
#include "nnue.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdbool.h>

int staticEval(bitboard *board, sidetomove tomove);

#endif
