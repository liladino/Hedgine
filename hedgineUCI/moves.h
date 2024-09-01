#ifndef MOVES_H
#define MOVES_H

#include "structs.h"
#include "hash.h"

#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

squarenums makesquarenum(square a);

move initializemove(squarenums from, squarenums to, int promotion);

void setmove(move *m, square from, square to, int promotion);

movelist *createMove(move m);

void firstmove(movelist **head, move m);

void insertMove(movelist *head, move m);

void undoMove(movelist **head);

void printmoves(movelist *head);

void printmove(move m);

void freemoves(movelist *head);

//bits:

void printBitPiece(u64 piece);

void printBitPieceAsBoard(u64 piece);

void printBitBoard(bitboard board);

void printBitBoard2d(bitboard board);

bitboard boardConvert(char board2d[12][12], int castling[4], squarenums enpass, bool tomove);

void addPieceOnBoard(char board2d[12][12], u64 piece, char piecesymbol);

void boardConvertBack(char board2d[12][12], bitboard board);

move boardConvertTomove(bitboard board1, bitboard board2, bool tomove);

#endif
