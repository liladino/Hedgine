#ifndef TESTS_H
#define TESTS_H

#include "structs.h"
#include "moves.h"
#include "legalmoves.h"
#include "movepicker.h"
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

void makePerfTest(bitboard* board, bool tomove);

bool testPseudoLegal();

bool testMovePicker();

void testLasker();

void testEnglish();

#endif
