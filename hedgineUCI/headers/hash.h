#ifndef HASH_H
#define HASH_H

#include "structs.h"
#include "moves.h"
#include "output.h"
#include "legalmoves.h"
#include "zobrrand.h"
#include "engine.h"

#ifdef DEBUG
	#include "debugmalloc.h"
	#include "output.h"
	//~ #include "input.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

TThashentry* allocTransTable(const unsigned int sizeInMB);

void setHashKey();

u64 hashPosition(const bitboard* const board, bool tomove);

TThashentry* lookup(u64 position);

void freeTransTable();

int readHashEntry(u64 hashValue, int remainingDepth, int depth, int alpha, int beta);

//~ move readHashEntryMove(const u64 pos);

void storePosTT(const u64 hashValue, int eval, evalflag flag, int depthRemaining, int depthSearched, const bitMove* const m);

void hashPieceIO(bitboard *board, int sq, int piece);

void hashTomove(bitboard *board);

void hashEnPassantIO(bitboard *board, int file);

void hashCastleO(bitboard *board, uint8_t mask);

void printBestLine(u64 pos, bool tomove);

void orderMoves(movearray* legalmoves);

void printHashEntry(u64 pos);

#ifdef DEBUG
void printCollisionStats();
#endif

void clearTransTable();

//~ void rmBestMoveFlag(u64 pos);

extern int g_RTwriteIndex;

void storeRepetiton(const u64 pos); 

void rmLastRepetition();

bool isRepetition(const u64 pos);

#endif
