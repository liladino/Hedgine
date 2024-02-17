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

void storePos(u64 pos, u64 nextpos, int eval, move m);

void hashPieceIO(bitboard *board, int sq, int piece);

void hashTomoveIO(bitboard *board);

void hashEnPassantIO(bitboard *board, int file);

void hashCastleO(bitboard *board, int dir);

void printBestLine(u64 pos);

void clearTransTable();
#endif
