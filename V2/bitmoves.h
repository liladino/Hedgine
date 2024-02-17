#ifndef BITMOVES_H
#define BITMOVES_H

#include "structs.h"
#include "bithash.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

void printBitPiece(u64 piece);

void printBitPieceAsBoard(u64 piece);

void printBitBoard(bitboard board);

void printBitBoard2d(bitboard board);

bitboard boardConvert(char board2d[12][12], int castling[4], squarenums enpass, bool tomove);

void addPieceOnBoard(char board2d[12][12], u64 piece, char piecesymbol);

void boardConvertBack(char board2d[12][12], bitboard board);

move boardConvertTomove(bitboard board1, bitboard board2, bool tomove);

#endif
