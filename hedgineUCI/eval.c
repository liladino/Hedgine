#include "eval.h"

/*
 * TABLE BONUSES
 * */

int openingking[8][8] = {
	{ 100, 105, 100, -40, 100, -40, 100, 105}, 
	{  50,   0,   0,   0,   0,   0,   0,  50},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
	{   0,   0,   0,   0,   0,   0,   0,   0},
};

int middleking[8][8] = {
	{  60,  60,  50,   0,   0,  40,  50,  50}, 
	{  50,  40,   0,   0,   0,   0,  40,  50},
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
	{   1,  15,   9,  10,  10,   9,  15,   1}, //2
	{   2,   9,  14,  10,  10,  14,   9,   2}, //3
	{   3,  10,  15,  18,  18,  15,  10,   3}, //4
	{   3,  15,  15,  18,  18,  15,  15,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int middlebishop[8][8] = {
	{   0,   1,   2,   3,   3,   2,   1,   0}, //1
	{   1,  15,   9,  10,  10,   9,  15,   1}, //2
	{   2,   9,  14,  15,  15,  14,   9,   2}, //3
	{   3,  10,  15,  18,  18,  15,  10,   3}, //4
	{   3,  15,  15,  18,  18,  15,  15,   3}, //5
	{   2,   9,  14,  15,  15,  14,   9,   2}, //6
	{   1,   4,   9,  10,  10,   9,   4,   1}, //7
	{   0,   1,   2,   3,   3,   2,   1,   0}, //8
};

int openingpawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{  10,  10,  10,   0,   0,  10,  10,  10}, //2
	{  10,  10,  10,  20,  20,   0,   0,  10}, //3
	{   0,   0,  10,  20,  20,  10,   0,   0}, //4
	{   0,   0,   0,  20,  20,   0,   0,   0}, //5
	{   0,   0,   0,   0,   0,   0,   0,   0}, //6
	{  50,  50,  50,  50,  50,  50,  50,  50}, //7
	{   0,   0,   0,   0,   0,   0,   0,   0}, //8
};

int middlepawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,  10,   0,   0,   0,   0,  10,   0}, //2
	{   0,   0,  10,  10,  10,   0,   0,   0}, //3
	{   0,   0,  10,  10,  10,  10,   0,   0}, //4
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
	{   0,   0,  40,  50,  50,  40,   0,   0}, //1
	{   0,   0,  10,  20,  20,  10,   0,   0}, //2
	{   0,   0,  10,  20,  20,  10,   0,   0}, //3
	{  10,  10,  10,  30,  30,  10,  10,  10}, //4
	{  10,  10,  10,  30,  30,  10,  10,  10}, //5
	{  30,  30,  30,  30,  30,  30,  30,  30}, //6
	{  50,  50,  50,  50,  50,  50,  50,  50}, //7
	{  50,  50,  50,  50,  50,  50,  50,  50}, //8
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
 
static int max(int a, int b){
	return (a > b ? a : b);
}

static int countPieces(const bitboard* const board){
	u64 all = (board->piece[wqueen] | board->piece[wbishop] | board->piece[wknight] | board->piece[wrook] | board->piece[bqueen] | board->piece[bbishop] | board->piece[bknight] | board->piece[brook]);
	u64 mask = 1;
	int count = 0;
	for (int i = 0; i < 64; i++){
		if (mask & all) count++;
		mask = mask << 1;
	}
	return count;
}

static int countFriendlyPieces(const bitboard* const board, bool tomove){
	u64 all;
	if (white == tomove) all = (board->piece[wqueen] | board->piece[wbishop] | board->piece[wknight] | board->piece[wpawn]);
	else all = (board->piece[bpawn] | board->piece[bqueen] | board->piece[bbishop] | board->piece[bknight] | board->piece[brook]);
	
	u64 mask = 1;
	int count = 0;
	for (int i = 0; i < 64; i++){
		if (mask & all) count++;
		mask = mask << 1;
	}
	return count;
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
	if (blendUpperLimit > 14) blendUpperLimit = 14; //if there are more pieces on the board (promotion)
	blendRate = max(blendLowerLimit, blendRate);
	
	return ((blendUpperLimit - blendRate) * value1 + (blendRate - blendLowerLimit) * value2) / (blendUpperLimit - blendLowerLimit);
}

static inline int sideEval(const bitboard* const board, bool tomove){
	int offset = (tomove == white ? 0 : bking);
	u64 piecemask = 1;
	int eval = 0;
	const int pieces = countPieces(board);
	const int friendlypieces = countFriendlyPieces(board, tomove);
	
	#define OPENING_PIECE_COUNT 9
	#define MIDDLE_PIECE_COUNT 6
	#define MAX_PIECE_COUNT 14
	
	bool opening = (pieces >= 9), middlegame = (pieces >= 6);
	
	for (int i = (tomove == white ? 0 : 7); (tomove == white ? i < 8 : i >= 0); i += (tomove == white ? 1 : -1)){
		for (int j = 0; j < 8; j++){
			if (board->piece[wpawn + offset] & piecemask){
				eval += 100;
				if (opening) {
					eval += valueBlender(middlepawn[i][j], openingpawn[i][j], pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
				}
				else if (middlegame){
					eval += middlepawn[i][j];
				}
				else {
					eval += valueBlender(endpawn[i][j], middlepawn[i][j], pieces, 1, MIDDLE_PIECE_COUNT);
				}
			}
			else if (board->piece[wknight + offset] & piecemask){
				eval += 310;
				if (opening) {
					eval += valueBlender(middleknight[i][j], openingknight[i][j], pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
				}
				else if (middlegame){
					eval += middleknight[i][j];
				}
				else {
					eval += valueBlender(endgamecenter[i][j], middleknight[i][j], pieces, 1, MIDDLE_PIECE_COUNT);
				}
			}
			else if (board->piece[wbishop + offset] & piecemask){
				eval += 313;
				if (opening) {
					eval += valueBlender(middlebishop[i][j], openingbishop[i][j], pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
				}
				else if (middlegame){
					eval += middlebishop[i][j];
				}
				else {
					eval += valueBlender(endgamecenter[i][j], middlebishop[i][j], pieces, 1, MIDDLE_PIECE_COUNT);
				}
			}
			else if (board->piece[wrook + offset] & piecemask){
				eval += 500;
				if (opening) {
					eval += valueBlender(middlerook[i][j], 0, pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
				}
				else if (middlegame){
					eval += middlerook[i][j];
				}
				else {
					//rook should have an open file detector for the endgame (and blended into middlegame)
					eval += valueBlender(0, middlerook[i][j], pieces, 1, MIDDLE_PIECE_COUNT);
				}
			}
			else if (board->piece[wking + offset] & piecemask){
				//a king safety bonus should be implemented
				if (opening) {
					eval += valueBlender(middleking[i][j], openingking[i][j], pieces, OPENING_PIECE_COUNT, MAX_PIECE_COUNT);
				}
				else if (middlegame){
					eval += middleking[i][j];
				}
				else {
					eval += valueBlender(endgamecenter[i][j], middleking[i][j], pieces, 2, MIDDLE_PIECE_COUNT) * 2 / (double)(friendlypieces + 1);
				}
			}
			else if (board->piece[wqueen + offset] & piecemask){
				eval += 900;
				/*if (pieces <= 14) {
					eval += ((14 - pieces) * endgamecenter[i][j] * 0.08);
				}*/
			}
			piecemask = piecemask << 1;
		}
	}
	//~ if (tomove == black) eval *= -1;
	
	return eval;
}

int fulleval(const bitboard* const board, bool tomove, int depth){
	//~ printBitBoard2d(*board);
	resultconst r = gameend(*board, tomove);
	if (r == draw) return draw;
	if (r != ongoing) return (blackwon - 100 + depth);
	return (sideEval(board, white) - sideEval(board, black)) * (tomove == white ? 1 : -1);
}
