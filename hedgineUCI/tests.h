#ifndef TESTS_H
#define TESTS_H

#include "structs.h"
#include "moves.h"
#include "legalmoves.h"
#include "hash.h"
#include "uci.h"
#include "engine.h"
#include "output.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool makePerfTestsAssert();

void makePerfTest(char board[12][12], bool tomove, int castling[4], squarenums enpass);

#endif
