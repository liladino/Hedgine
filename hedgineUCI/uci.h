#ifndef UCIFUNCTIONS_H
#define UCIFUNCTIONS_H

#include "hash.h"
#include "input.h"
#include "structs.h"
#include "engine.h"
#include "tests.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include "sys/time.h"

extern gameInfo info;

void initializeAll();

long int getTime_ms();
  
int inputWaiting();

void readInput();

void parsePosition(char *command, bitboard* bboard, bool *tomove, int* fmv, int* movenum);

void parseGo(char *command, bitboard* board, bool *tomove);

void UCIloop(bitboard* board, bool *tomove, int* fmv, int* movenum);

#endif
