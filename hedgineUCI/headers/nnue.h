#ifndef NNUE_H
#define NNUE_H

#include "structs.h"
#include "input.h"

#include <stdbool.h>

#define NNUE_FEATURES 768   /* 12 pieces * 64 squares */
#define NNUE_HIDDEN 128     /* must match the trained net, checked when loading */
#define NNUE_DEFAULT_FILE "net128.bin"

/**
 * Loads a net written by nnue/train.py. Returns false (and prints why) on failure. 
 */
bool NNUEload(const char *path);

/**
 * Centipawns from the side to move's point of view. 
 */
int NNUEeval(const bitboard *board, sidetomove tomove);

/**
 * The same without rounding, for comparing against the Python reference. 
 */
float NNUEevalFloat(const bitboard *board, sidetomove tomove);

/**
 * Reads "FEN | expected cp" lines (net.golden.txt) and compares NNUEevalFloat against them.
 */
bool NNUEgoldenTest(const char *goldenPath);

#endif
