#ifndef GAME_H
#define GAME_H

#include "input.h"
#include "legalmoves.h"
#include "moves.h"
#include "output.h"
#include "pgninput.h"
#include "pgnoutput.h"
#include "structs.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif
#include "engine.h"
#include "feninput.h"
#include "fenoutput.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

void playmoves(char board[12][12], movelist *head, int castling[], squarenums *enpass, int *fmv);

int playermove(char board[12][12], char *loadfile, movelist *game, movelist *legalmoves, move* m, bool *tomove, int castling[], squarenums *enpass, int *halfmove, int *movenum, int *fmv, int *view, int *boardcol, int playercol);

void resetBoard(char board[12][12]);

void save(char *loadfile, movelist *game, int playercol, int result);

void repetition_reset(char s_board[12][12], bool *s_tomove, int s_castling[4], squarenums *s_enpass, char *loadfile);

int game(int load, int players, char *lf_orig);

#endif
