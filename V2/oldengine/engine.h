#ifndef ENGINE_H
#define ENGINE_H

#include "output.h"
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


move CPU(int cpulvl, char board[12][12], bool tomove, int castling[4], squarenums enpass);


#endif
