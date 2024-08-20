#ifndef BITENGINE_H
#define BITENGINE_H

#include "structs.h"
#include "bitmoves.h"
#include "bitlegalmoves.h"
#include "bithash.h"

#ifdef DEBUG
	#include "debugmalloc.h"
	#include "output.h"
	#include "feninput.h"
#endif
#ifdef BENCH
	#include "feninput.h"
	#include "output.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>

#define millisec 1000
#define epsilon 0.00005
#define PosINF 2147483640
#define NegINF -2147483640
#define MAXSEARCHDEPTH 40

move lvl0(bitboard board, bool tomove);

move CPU(int cpulvl, char board[12][12], bool tomove, int castling[4], squarenums enpass);

void makeBenchTest(char board[12][12], bool tomove, int castling[4], squarenums enpass);

#endif
