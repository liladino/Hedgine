#ifndef ENGINE_H
#define ENGINE_H

#include "structs.h"
#include "moves.h"
#include "legalmoves.h"
#include "hash.h"
#include "uci.h"
#include "engine.h"
#include "eval.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

extern bool g_stopSearch;

bool isMateScore(int score);

move CPU(int cpulvl, bitboard bboard, bool tomove);

#endif
