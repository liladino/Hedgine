#ifndef NNUE_H
#define NNUE_H

#include "structs.h"
#include "input.h"

#include <stdbool.h>

#define NNUE_FEATURES 768   /* 12 pieces * 64 squares */
#define NNUE_HIDDEN 128     /* must match the trained net, checked when loading */

/**
 * Loads a net written by nnue/train.py. Returns false on failure, and keeps the
 * previously loaded net in that case.
 */
bool NNUEload(const char *path);

/**
 * Loads the net that was embedded in the executable at build time.
 */
bool NNUEloadEmbedded(void);

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

/**
 * Incremental accumulator (make NNUE=1 NNUE_INC=1)
 *
 * Without it every NNUEeval rebuilds both accumulators from all pieces on the
 * board. With it the search keeps a stack of accumulators: one per ply, each
 * made from its parent by applying only the features the move changed.
 *
 *   NNUEbeginSearch  builds accumulator 0 from scratch for the root position
 *   NNUEpush         after a legal makeMove: copy the top, apply the move's changes
 *   NNUEpop          next to undoMove: drop the top, the parent is still intact
 *   NNUEendSearch    NNUEeval goes back to rebuilding from the board
 *
 * While a search is active NNUEeval ignores its board argument and reads the
 * top of the stack, so pushes and pops must follow makeMove / undoMove exactly.
 */
#if defined(NNUE_INCREMENTAL) && !defined(USE_NNUE)
	#error "NNUE_INCREMENTAL needs USE_NNUE"
#endif

#ifdef NNUE_INCREMENTAL
	void NNUEbeginSearch(const bitboard *board);
	void NNUEendSearch(void);
	void NNUEpush(const bitMove *m, const bitUndo *u);
	void NNUEpop(void);

	/**
	 * Plays random games from a few positions, pushing / popping the accumulator
	 * along the way, and compares it with a full rebuild after every step.
	 */
	bool NNUEincrementalTest(void);

	#define NNUE_BEGIN_SEARCH(board) NNUEbeginSearch(board)
	#define NNUE_END_SEARCH()        NNUEendSearch()
	#define NNUE_PUSH(m, u)          NNUEpush((m), (u))
	#define NNUE_POP()               NNUEpop()
#else
	#define NNUE_BEGIN_SEARCH(board) ((void)0)
	#define NNUE_END_SEARCH()        ((void)0)
	#define NNUE_PUSH(m, u)          ((void)0)
	#define NNUE_POP()               ((void)0)
#endif

#endif
