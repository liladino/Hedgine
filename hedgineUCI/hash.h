#ifndef HASH_H
#define HASH_H

#include "structs.h"
#include "moves.h"
#include "legalmoves.h"
#include "mtwister.h"

#ifdef DEBUG
	#include "debugmalloc.h"
	#include "output.h"
	//~ #include "input.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void setHashKey();

u64 hashPosition(const bitboard* board, bool tomove);

hashentry* lookup(u64 position);

int readHashEntry(const u64 pos, int* alpha, int* beta, const int depth, const int maxdepth, const int oddity);

//~ move readHashEntryMove(const u64 pos);

void storePos(const u64 pos, const int eval, const evalflag flag, const int depth/*, const move m, const u64 next*/);

void hashPieceIO(bitboard *board, int sq, int piece);

void hashTomoveIO(bitboard *board);

void hashEnPassantIO(bitboard *board, int file);

void hashCastleO(bitboard *board, int dir);

void printBestLine(u64 pos, bool tomove);

void orderMoves(movearray* legalmoves);

void printHashEntry(u64 pos);

#ifdef DEBUG
void printCollisionStats();
#endif

void clearTransTable();

void rmBestMoveFlag(u64 pos);

#endif
