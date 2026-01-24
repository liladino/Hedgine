#ifndef OUTPUT_H
#define OUTPUT_H

#include "structs.h"
#include "moves.h"

#include <stdio.h>
#include <stdbool.h>

void printboardLetters(FILE* ostream, char board[12][12]);

void printBitBoard2d(FILE* ostream, bitboard board);

void printBitBoard(bitboard board);

void printBitPiece(u64 piece);

void printBitPieceAsBoard(u64 piece);

void printmove(FILE* ostream, move m);

#ifdef DEBUG
void printLegalmoves(movearray legalmoves, bitboard board, bool tomove);
#endif


#endif
