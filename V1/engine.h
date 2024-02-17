#ifndef ENGINE_H
#define ENGINE_H

//#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"
//#include "input.h"
#include "legalmoves.h"
#include "moves.h"
#include "output.h"
//#include "pgninput.h"
#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

#define millisec 1000
#define epsilon 0.00005


double fabs(double a);

int piecedistance(char board[12][12], char piece1, char piece2);

double openingphase(char board[12][12]);

double middlephase(char board[12][12]);

double endgamephase(char board[12][12], int count);

double basic_eval(char board[12][12]);

move lvl0(movelist *legalmoves);

double fulleval(char board[12][12], bool tomove, int castling[4], squarenums enpass);

int countpieces(char board[12][12]);

void copymeta(char board[12][12], int castling[4], squarenums *enpass, char newboard[12][12], int newcastling[4], squarenums *newenpass);

double min(double a, double b);

double max(double a, double b);

int intmax(int a, int b);

movewitheval CLCEngine(char board[12][12], bool tomove, int castling[4], squarenums enpass, int depth, int maxdepth, double alpha, double beta);

move CPU(int cpulvl, char board[12][12], bool tomove, int castling[4], squarenums enpass);


#endif
