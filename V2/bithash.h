#ifndef BITHASH_H
#define BITHASH_H

#include "structs.h"
#include "bitmoves.h"
#include "moves.h"
#include "bitlegalmoves.h"
#include "random.h"

#ifdef DEBUG
	#include "debugmalloc.h"
	#include "output.h"
	#include "feninput.h"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void setHashKey();

u64 hashPosition(bitboard board, bool tomove);

hashentry* lookup(u64 position);

int readHashEntry(const u64 pos, const int alpha, const int beta, const int depth);

void storePos(u64 pos, int eval, evalflag flag, int depth, move m, u64 next);

void hashPieceIO(bitboard *board, int sq, int piece);

void hashTomoveIO(bitboard *board);

void hashEnPassantIO(bitboard *board, int file);

void hashCastleO(bitboard *board, int dir);

void printBestLine(u64 pos);

#endif
