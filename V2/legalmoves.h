#ifndef LEGALMOVES_H
#define LEGALMOVES_H

//#include "engine.h"
//#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"
//#include "input.h"

#include "moves.h"
//#include "output.h"
//#include "pgninput.h"
//#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdbool.h>

int signofn(int n);

int abs(int a);

void resetBoard(char board[12][12]);

void makemove(char board[12][12], move m);

void setmeta(char board[12][12], move m, int castling[], squarenums *enpass, int *fmv);

int incheck(char board[][12], bool tomove);

void copyBoard(char from[12][12], char to[12][12]);

void addmove(char board[12][12], char newboard[12][12], movelist *legalmoves, bool tomove, move m);

int checkfordraw(char board[12][12]);

void addpawnmoves(char board[12][12], movelist *legalmoves, bool tomove, squarenums enpass, int i, int j);


void addknightmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j);

void addbishopmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j);

void addrookmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j);

void addqueenmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j);

void addkingmoves(char board[12][12], movelist *legalmoves, bool tomove, int castling[] , int i, int j);

int generatemoves(char board[12][12], movelist *legalmoves, bool tomove, int castling[4], squarenums enpass );

bool endofgame(char board[12][12], movelist *legalmoves, bool tomove, int castling[4], squarenums enpass );

bool compareboards(char board1[12][12], char board2[12][12]);

bool comparecastle(int castling1[4], int castling2[4]);

bool repetition(char startboard[12][12], movelist *start, bool s_tomove, int s_castling[4], squarenums s_enpass, char board[12][12], bool tomove, int castling[4], squarenums enpass);

#endif
