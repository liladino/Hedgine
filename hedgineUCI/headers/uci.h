#ifndef UCIFUNCTIONS_H
#define UCIFUNCTIONS_H

#include "hash.h"
#include "input.h"
#include "structs.h"
#include "engine.h"
#include "tests.h"

#ifdef WIN32
#include "windows.h"
#endif

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include "sys/time.h"

#define MOVE_OVERHEAD_DEF 30
#define MOVE_OVERHEAD_MIN 0
#define MOVE_OVERHEAD_MAX 5000

extern gameInfo info;

void initializeAll();

long int getTime_ms();
  
int inputWaiting();

void readInput();

void parsePosition(const char *command, bitboard* bboard, sidetomove *tomove, int* fmv, int* movenum);

void parseGo(const char *command, const bitboard* board, const sidetomove *tomove, int fmv);

void UCIloop(bitboard* board, sidetomove *tomove, int* fmv, int* movenum);

void setMoveTime(int increment);

#endif
