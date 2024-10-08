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

#define SECOND 1000
#define PosINF 2147483640
#define NegINF -2147483640
#define MAXSEARCHDEPTH 40

extern bool stopSearch;
extern move PV[MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];

move lvl0(bitboard board, bool tomove);

move CPU(int cpulvl, bitboard bboard, bool tomove);

void makeBenchTest(char board[12][12], bool tomove, int castling[4], squarenums enpass);

void setMoveTime(int increment);

#endif
