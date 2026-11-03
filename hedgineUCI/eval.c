#include "headers/eval.h"

/*
 * TABLE BONUSES
 * */

int openingking[8][8] = {
	{  10,  20,  10,   0,   0,   0,  20,  10}, 
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
};

int middleking[8][8] = {
	{  50,  50,  40,   0,   0,  10,  50,  50}, 
	{  40,  40,   0,   0,   0,   0,  40,  50},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
};

int openingknight[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0,   0,  10,  10,   0,   0,   0}, //2
	{   0,   3,  15,  10,  10,  15,   3,   0}, //3
	{   0,  10,  15,  18,  18,  15,  10,   0}, //4
	{   3,  10,  15,  18,  18,  15,  10,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int middleknight[8][8] = {
	{   0,   1,   2,   3,   3,   2,   1,   0}, //1
	{   1,   4,   9,  10,  10,   9,   4,   1}, //2
	{   2,   9,  15,  15,  15,  15,   9,   2}, //3
	{   3,  10,  15,  18,  18,  15,  10,   3}, //4
	{   3,  10,  15,  18,  18,  15,  10,   3}, //5
	{   2,  10,  15,  18,  18,  15,  10,   2}, //6
	{   1,   4,  15,  10,  10,  15,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int openingbishop[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{  14,  15,   9,  10,  10,   9,  15,  14}, //2
	{   9,   9,  14,   0,   0,  14,   9,   9}, //3
	{   3,  15,  15,   5,   5,  15,  15,   3}, //4
	{   3,  15,  15,   5,   5,  15,  15,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int middlebishop[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{  14,  15,   9,  10,  10,   9,  15,  14}, //2
	{   9,   9,  14,   9,   9,  14,   9,   9}, //3
	{   3,  15,  15,  18,  18,  15,  15,   3}, //4
	{   3,  15,  15,  18,  18,  15,  15,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int openingpawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{  10,  10,  10,   1,   1,  10,  10,  10}, //2
	{  10,  10,  10,  10,  10, -10,   0,  10}, //3
	{   0,   0,  10,  12,  12,  10,   0,   0}, //4
	{   0,   0,   0,  20,  20,   0,   0,   0}, //5
	{   0,   0,   0,   0,   0,   0,   0,   0}, //6
	{  50,  50,  50,  50,  50,  50,  50,  50}, //7
	{   0,   0,   0,   0,   0,   0,   0,   0}, //8
};

int middlepawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   5,   5,   0,   0,   0,   0,   5,   5}, //2
	{   5,   5,  10,  10,  10,   0,   5,   5}, //3
	{   0,   0,  10,  12,  12,  10,   0,   0}, //4
	{   0,   0,  10,  20,  20,  10,   0,   0}, //5
	{  30,  30,  30,  30,  30,  30,  30,  30}, //6
	{  50,  50,  50,  50,  50,  50,  50,  50}, //7
	{   0,   0,   0,   0,   0,   0,   0,   0}, //8
};

int endpawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0,   0,   0,   0,   0,   0,   0}, //2
	{   0,   0,   0,   0,   0,   0,   0,   0}, //3
	{  10,  10,  10,  10,  10,  10,  10,  10}, //4
	{  20,  20,  20,  20,  20,  20,  20,  20}, //5
	{  30,  30,  30,  30,  30,  30,  30,  30}, //6
	{  50,  50,  50,  50,  50,  50,  50,  50}, //7
	{   0,   0,   0,   0,   0,   0,   0,   0}, //8
};

int middlerook[8][8] = {
	{   0,   0,  30,  40,  40,  30,   0,   0}, //1
	{   0,   0,  10,  20,  20,  10,   0,   0}, //2
	{   0,   0,  10,  20,  20,  10,   0,   0}, //3
	{  10,  10,  10,  30,  30,  10,  10,  10}, //4
	{  10,  10,  10,  30,  30,  10,  10,  10}, //5
	{  30,  30,  30,  30,  30,  30,  30,  30}, //6
	{  40,  40,  40,  40,  40,  40,  40,  40}, //7
	{  40,  40,  40,  40,  40,  40,  40,  40}, //8
};

int endgamecenter[8][8] = {
	{   0,   1,   2,   3,   3,   2,   1,   0}, //1
	{   1,   4,   9,  10,  10,   9,   4,   1}, //2
	{   2,   9,  14,  15,  15,  14,   9,   2}, //3
	{   3,  10,  15,  16,  16,  15,  10,   3}, //4
	{   3,  10,  15,  16,  16,  15,  10,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

/*
 * EVALUATION
 * */
 
static int i_max(int a, int b){
	return (a > b ? a : b);
}

static inline int i_min(int a, int b){
	if (a < b) return a;
	return b;
}

static inline int counter(u64 var){
    return __builtin_popcountll(var);  
}

int countPieces(const bitboard* const board){
	u64 all = (board->piece[wqueen] | board->piece[wbishop] | board->piece[wknight] | board->piece[wrook] | board->piece[bqueen] | board->piece[bbishop] | board->piece[bknight] | board->piece[brook]);
	return counter(all); 
}

int countFriendlyPieces(const bitboard* const board, bool tomove){
	u64 all;
	if (white == tomove) all = (board->piece[wpawn] | board->piece[wqueen] | board->piece[wbishop] | board->piece[wknight] | board->piece[wrook]);
	else                 all = (board->piece[bpawn] | board->piece[bqueen] | board->piece[bbishop] | board->piece[bknight] | board->piece[brook]);
	return counter(all);
}

/* Provides a transition between two values.
 * E.g. the two values are 10 and 50, the piececunt (the blendRate) is 11, and 
 * the upper and lower limits are 9 and 14. The new value is then 
 * ((14 - 11) * 10 + (11 - 9) *  50) / (14 - 9) 
 * 
 * if belndrRate == blendLowerLimit, it should return value1, and if
 * blendRate == blendUpperLimit, it should return value2.
 * 
 * blendRate should always be in range of the limits.
 * */
static inline int valueBlender(int value1, int value2, int blendRate, int blendLowerLimit, int blendUpperLimit){
	if (blendUpperLimit < blendLowerLimit) {
		int temp = blendUpperLimit;
		blendUpperLimit = blendLowerLimit;
		blendLowerLimit = temp;
	}
	//clamp, e.g. there can be more than 14 pieces on the board after promotions
	blendRate = i_max(blendLowerLimit, blendRate);
	blendRate = i_min(blendUpperLimit, blendRate);
	
	return ((blendUpperLimit - blendRate) * value1 + (blendRate - blendLowerLimit) * value2) / (blendUpperLimit - blendLowerLimit);
}

#define OPENING_PIECE_COUNT 9
#define MIDDLE_PIECE_COUNT 6
#define MAX_PIECE_COUNT 14

/* Table bonus of one piece in the current game phase:
 * opening: blends from the middlegame value into the opening value as pieces grow
 * middlegame: the middlegame value
 * endgame: blends from the endgame value into the middlegame value as pieces grow
 * */
static inline int phaseValue(int openingValue, int middleValue, int endValue, int pieces, bool opening, bool middlegame){
	if (opening) {
		return valueBlender(middleValue, openingValue, pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
	}
	if (middlegame){
		return middleValue;
	}
	return valueBlender(endValue, middleValue, pieces, 1, MIDDLE_PIECE_COUNT);
}

/* The tables are from white's perspective (row 0 = rank 1),
 * black's pieces are mirrored vertically.
 * */
static inline int tableRow(int square, bool tomove){
	return (tomove == white ? square / 8 : 7 - square / 8);
}

static inline int sideEval(const bitboard* const board, bool tomove){
	int offset = (tomove == white ? 0 : bking);
	int eval = 0;
	const int pieces = countPieces(board);

	bool opening = (pieces >= OPENING_PIECE_COUNT), middlegame = (pieces >= MIDDLE_PIECE_COUNT);
	int pawncount = __builtin_popcountll((board->piece[wpawn] | board->piece[bpawn]));

	u64 curr = board->piece[wpawn + offset];
	while (curr) {
		int square = __builtin_ctzll(curr);
		curr &= curr - 1;
		int i = tableRow(square, tomove), j = square % 8;

		eval += 100 + phaseValue(openingpawn[i][j], middlepawn[i][j], endpawn[i][j], pieces, opening, middlegame);
	}

	curr = board->piece[wknight + offset];
	while (curr) {
		int square = __builtin_ctzll(curr);
		curr &= curr - 1;
		int i = tableRow(square, tomove), j = square % 8;

		eval += 310 + pawncount / 6
			+ phaseValue(openingknight[i][j], middleknight[i][j], endgamecenter[i][j], pieces, opening, middlegame);
	}

	curr = board->piece[wbishop + offset];
	while (curr) {
		int square = __builtin_ctzll(curr);
		curr &= curr - 1;
		int i = tableRow(square, tomove), j = square % 8;

		eval += 318 - pawncount / 3
			+ phaseValue(openingbishop[i][j], middlebishop[i][j], endgamecenter[i][j], pieces, opening, middlegame);
	}

	curr = board->piece[wrook + offset];
	while (curr) {
		int square = __builtin_ctzll(curr);
		curr &= curr - 1;
		int i = tableRow(square, tomove), j = square % 8;

		//no opening and endgame tables, those blend to 0
		eval += 500 + 16 - pawncount
			+ phaseValue(0, middlerook[i][j], 0, pieces, opening, middlegame);
	}

	eval += 900 * counter(board->piece[wqueen + offset]);

	//a king safety bonus should be implemented
	curr = board->piece[wking + offset];
	if (curr) {
		int square = __builtin_ctzll(curr);
		int i = tableRow(square, tomove), j = square % 8;

		if (opening || middlegame) {
			eval += phaseValue(openingking[i][j], middleking[i][j], 0, pieces, opening, middlegame);
		}
		else {
			//the endgame king blends from a lower limit of 2, and is scaled by the friendly pieces
			const int friendlypieces = countFriendlyPieces(board, tomove);
			eval += valueBlender(endgamecenter[i][j], middleking[i][j], pieces, 2, MIDDLE_PIECE_COUNT) * 2 / (double)(friendlypieces + 1);
		}
	}

	return eval;
}

static inline int signum(int x){
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

static inline int kingDist(u64 wk, u64 bk){
	int w = __builtin_ctzll(wk);
	int b = __builtin_ctzll(bk);
	
	return i_min(abs(w/8 - b/8), abs((w & 7) - (b & 7)));
}

int staticEval(bitboard *board, bool tomove)
{
    int score = sideEval(board, white) - sideEval(board, black);
    return tomove == white ? score : -score;
	
	// printBitBoard2d(stdout, *board);
}