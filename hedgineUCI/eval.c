#include "eval.h"

/*
 * TABLE BONUSES
 * */

int openingking[8][8] = {
	{  90, 100,  30,   0,  30, -30, 100, 100}, 
	{ -50, -80, -80, -80, -80,-100, -80, -50},
	{-100,-150,-150,-200,-200,-150,-150,-100},
	{-200,-200,-200,-300,-300,-200,-200,-200},
	{-200,-300,-400,-400,-400,-400,-300,-200},
	{-400,-400,-400,-400,-400,-400,-400,-300},
	{-600,-500,-450,-450,-450,-450,-500,-600},
	{-700,-600,-500,-450,-450,-500,-600,-700},
};

int middleking[8][8] = {
	{  50,  50,  10, -30, -30,  30,  50,  50}, 
	{ -30, -30, -50, -80, -80, -50, -30, -30},
	{-100,-150,-150,-200,-200,-150,-150,-100},
	{-200,-200,-200,-300,-300,-200,-200,-200},
	{-200,-300,-400,-400,-400,-400,-300,-200},
	{-400,-400,-400,-400,-400,-400,-400,-300},
	{-600,-500,-450,-450,-450,-450,-500,-600},
	{-700,-600,-500,-450,-450,-500,-600,-700},
};

int openingknight[8][8] = {
	{ -10, -10, -10, -10, -10, -10, -10, -10}, //1
	{   0,   0,   0,   9,  10,   0,   0,   0}, //2
	{   0,  10,  10,   0,   0,  20,  10,   0}, //3
	{  20,   0,  40,  60,  60,  40,   0,  20}, //4
	{  30,  30,  55,  39,  39,  55,  30,  30}, //5
	{  30,  50,  60,  60,  60,  60,  50,  30}, //6
	{  50,  60, 100,  70,  70, 100,  60,  50}, //7
	{  30,  30,  30,  30,  30,  30,  30,  30}, //8
};

int middleknight[8][8] = {
	{ -50, -10, -40,   0,   0, -40, -10, -50}, //1
	{   0,   0,  10,  40,  40,  10,   0,   0}, //2
	{  10,  20,  45,   0,   0,  45,  20,  10}, //3
	{  20,   0,  50,  55,  55,  50,   0,  20}, //4
	{  20,  30,  55,  55,  55,  55,  30,  20}, //5
	{  30,  50,  60,  60,  60,  60,  50,  30}, //6
	{  50,  60,  60,  70,  70,  60,  60,  50}, //7
	{  10,  30,  30,  30,  30,  30,  30,  10}, //8
};

int openingbishop[8][8] = {
	{   0, -20,  -5, -20, -20,  -5, -20,   0}, //1
	{  20,  50,   0,  21,  21,   0,  50,  20}, //2
	{ -10,  20,  10,   0,   0,  10,  20, -10}, //3
	{  30, -10,  35,  20,  20,  35, -10,  30}, //4
	{  20,  20,   0,  20,  20,   0,  31,  20}, //5
	{   3,  10,  10,  10,  10,  10,  10,  30}, //6
	{  20,  10,  10,  10,  10,  10,  10,  20}, //7
	{   0,   0,   0,   0,   0,   0,   0,   0}, //8
};

int middlebishop[8][8] = {
	{  10,   0, -10, -10, -10, -10,   0,  10}, //1
	{  20,  50,   0,  20,  20,   0,  50,  20}, //2
	{ -10,  20,  10,  20,  20,  10,  20, -10}, //3
	{  40, -10,  50,  20,  20,  50, -10,  40}, //4
	{  20,  40,   0,  20,  20,   0,  40,  20}, //5
	{  40,  10,  10,  10,  10,  10,  10,  40}, //6
	{  20,  10,  10,  10,  10,  10,  10,  20}, //7
	{  10,   0,   0,   0,   0,   0,   0,  10}, //8
};

int openingpawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0, -10, -20, -20,  10,   0,   0}, //2
	{  10,  40,  10,  40,  40,   0,   0,  10}, //3
	{   0, -30,  50,  50,  40,  10, -30,   0}, //4
	{   0,   0,   0,  45,  45,   0,   0,   0}, //5
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //6
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //7
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //8
};

int middlepawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0,   0,   0,   0,  10,   0,   0}, //2
	{  10,  10,  10,  20,  20,   0,   0,  10}, //3
	{  10,   0,  40,  50,  50,   0,   0,  10}, //4
	{  10,  10,  20,  50,  50,  10,  10,  10}, //5
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //6
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //7
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //8
};

int endpawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0,   0,   0,   0,   0,   0,   0}, //2
	{  20,  20,  20,  20,  20,  20,  20,  20}, //3
	{  50,  50,  50,  50,  50,  50,  50,  50}, //4
	{  80,  80,  80,  80,  80,  80,  80,  80}, //5
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //6
	{ 300, 300, 300, 300, 300, 300, 300, 300}, //7
	{1000,1000,1000,1000,1000,1000,1000,1000}, //8
};

int middlerook[8][8] = {
	{   0,   0,  40,  50,  50,  30,   0,   0}, //1
	{   0,   0,  10,  20,  20,  10,   0,   0}, //2
	{   0,   0,  10,  20,  20,  10,   0,   0}, //3
	{  10,  10,  10,  30,  30,  10,  10,  10}, //4
	{  10,  10,  10,  30,  30,  10,  10,  10}, //5
	{  50,  50,  50,  50,  50,  50,  50,  50}, //6
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //7
	{  50,  50,  50,  50,  50,  50,  50,  50}, //8
};

int endgamecenter[8][8] = {
	{   0,   1,   2,   3,   3,   2,   1,   0}, //1
	{   1,   4,   9,  10,  10,   9,   4,   1}, //2
	{   2,   9,  14,  15,  15,  14,   9,   2}, //3
	{   3,  10,  15,  18,  18,  15,  10,   3}, //4
	{   3,  10,  15,  18,  18,  15,  10,   3}, //5
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

static int sideEval(const bitboard* const board, bool tomove){
	int offset = (tomove == white ? 0 : bking);
	u64 piecemask = 1;
	int eval = 0;
	const int pieces = countPieces(board);
	const int friendlypieces = countFriendlyPieces(board, tomove);
	
	for (int i = (tomove == white ? 0 : 7); (tomove == white ? i < 8 : i >= 0); i += (tomove == white ? 1 : -1)){
		for (int j = 0; j < 8; j++){
			if (board->piece[wpawn + offset] & piecemask){
				eval += 100;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingpawn[i][j] * 0.2 + (14 - pieces) * middlepawn[i][j] * 0.2);
				}
				else if (pieces > 5){
					eval += middlepawn[i][j] + 10;
				}
				else {
					eval += (max(pieces - 1, 0) * middlepawn[i][j] * 0.2 + (5 - pieces) * endpawn[i][j] * 0.5);
				}
			}
			else if (board->piece[wknight + offset] & piecemask){
				eval += 310;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingknight[i][j] * 0.2 + (14 - pieces) * middleknight[i][j] * 0.2) + 15;
				}
				else if (pieces > 5){
					eval += middleknight[i][j];
				}
				else {
					eval += (max(pieces - 1, 0) * middleknight[i][j] * 0.2 + (5 - pieces) * endgamecenter[i][j] * 0.5);
				}
			}
			else if (board->piece[wbishop + offset] & piecemask){
				eval += 313;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingbishop[i][j] * 0.2 + (14 - pieces) * middlebishop[i][j] * 0.2);
				}
				else if (pieces > 5){
					eval += middlebishop[i][j] + 15;
				}
				else {
					eval += (max(pieces - 1, 0) * middlebishop[i][j] * 0.2 + (5 - pieces) * endgamecenter[i][j] * 0.5) + 20;
				}
			}
			else if (board->piece[wrook + offset] & piecemask){
				eval += 500;
				if (pieces > 8) {
					eval += ((pieces - 9) * 500 * 0.2 + (14 - pieces) * middlerook[i][j] * 0.2) - 10;
				}
				else if (pieces > 6){
					eval += middlerook[i][j];
				}
				else {
					eval += (max(pieces - 1, 0) * middlerook[i][j] * 0.2 + (6 - pieces) * endgamecenter[i][j] * 0.5);
				}
			}
			else if (board->piece[wking + offset] & piecemask){
				if (pieces > 8) {
					eval += ((pieces - 9) * openingking[i][j] * 0.2 + (14 - pieces) * middleking[i][j] * 0.2) - 10;
				}
				else if (pieces > 6){
					eval += middleking[i][j];
				}
				else {
					eval += ((max(pieces - 1, 0) * middleking[i][j] * 0.2 + (6 - pieces) * endgamecenter[i][j]) / (double)(friendlypieces + 1));
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
	if (tomove == black) eval *= -1;
	
	return eval;
}

int fulleval(const bitboard* const board, bool tomove){
	resultconst r = gameend(*board, tomove);
	if (r != ongoing) return (tomove == white ? r : -r);
	return (sideEval(board, white) + sideEval(board, black)) * (tomove == white ? 1 : -1);
}
