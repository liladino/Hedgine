#ifndef INPUT_H
#define INPUT_H

//#include "engine.h"
//#include "feninput.h"
//#include "fenoutput.h"
//#include "game.h"

#include "legalmoves.h"
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
#include <stdlib.h>
#include <string.h>

bool checkiflegal(move m, movelist *legalmoves);

bool setpiecefound(int *piecefound, square from, square to, movelist *legalmoves );

int SANconvertPawn(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int SANconvertKing(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

bool arethesame(char *SAN_move, char *castle);

int SANconvertCastle(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int ifTakesSetTo(char *SAN_move, int x, square *to);

int setFromAndTo(char *SAN_move, square *to, square *from);

int checkpiece(int i, char j, square possiblefrom);

int SANconvertKnight(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int SANconvertRook(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int SANconvertBishop(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int SANconvertQueen(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves );

int gothroughpieces(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves);
	
int fromSANconvert(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves);

#endif
