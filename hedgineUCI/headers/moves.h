#ifndef MOVES_H
#define MOVES_H

#include "structs.h"
#include "hash.h"
#include "output.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

squarenums makesquarenum(square a);

move initializemove(squarenums from, squarenums to, int promotion);

void setmove(move *m, square from, square to, int promotion);

//bits:

int makeMoveIfLegal(bitboard* board, const bool tomove, const move m);

bitboard boardConvert(char board2d[12][12], int castling[4], squarenums enpass, bool tomove);

void addPieceOnBoard(char board2d[12][12], u64 piece, char piecesymbol);

void boardConvertBack(char board2d[12][12], bitboard board);

bool compareBitMoveToMove(const bitMove bm, const move m);

move convertBitMoveToMove(const bitMove m);

bitMove convertMoveToBitMove(const bitboard* const board, bool tomove, const move m);

//~ move boardConvertTomove(const bitboard* const board1, const bitboard* const board2, bool tomove);

bool lastMoveWasCapture(const bitboard* const lastboard, const move m, const bool tomove);

void setMailBox(const bitboard* const board);

void printMailBox();

bitUndo makeMove(bitboard* board, bitMove m);

void undoMove(bitboard* board, const bitMove m, const bitUndo u);

int attackValue(int from, int to);

#endif
