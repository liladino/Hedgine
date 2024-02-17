#ifndef MOVES_H
#define MOVES_H

//#include "engine.h"
//#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"
//#include "input.h"
//#include "legalmoves.h"

//#include "output.h"
//#include "pgninput.h"
//#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>

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


#endif
