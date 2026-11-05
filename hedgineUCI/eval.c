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

u64 bPawnNeighborBonus[] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 131584ULL, 328960ULL, 657920ULL, 1315840ULL, 2631680ULL, 5263360ULL, 10526720ULL, 4210688ULL, 33685504ULL, 84213760ULL, 168427520ULL, 336855040ULL, 673710080ULL, 1347420160ULL, 2694840320ULL, 1077936128ULL, 8623489024ULL, 21558722560ULL, 43117445120ULL, 86234890240ULL, 172469780480ULL, 344939560960ULL, 689879121920ULL, 275951648768ULL, 2207613190144ULL, 5519032975360ULL, 11038065950720ULL, 22076131901440ULL, 44152263802880ULL, 88304527605760ULL, 176609055211520ULL, 70643622084608ULL, 565148976676864ULL, 1412872441692160ULL, 2825744883384320ULL, 5651489766768640ULL, 11302979533537280ULL, 22605959067074560ULL, 45211918134149120ULL, 18084767253659648ULL, 144678138029277184ULL, 361695345073192960ULL, 723390690146385920ULL, 1446781380292771840ULL, 2893562760585543680ULL, 5787125521171087360ULL, 11574251042342174720ULL, 4629700416936869888ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};
u64 wPawnNeighborBonus[] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 514ULL, 1285ULL, 2570ULL, 5140ULL, 10280ULL, 20560ULL, 41120ULL, 16448ULL, 131584ULL, 328960ULL, 657920ULL, 1315840ULL, 2631680ULL, 5263360ULL, 10526720ULL, 4210688ULL, 33685504ULL, 84213760ULL, 168427520ULL, 336855040ULL, 673710080ULL, 1347420160ULL, 2694840320ULL, 1077936128ULL, 8623489024ULL, 21558722560ULL, 43117445120ULL, 86234890240ULL, 172469780480ULL, 344939560960ULL, 689879121920ULL, 275951648768ULL, 2207613190144ULL, 5519032975360ULL, 11038065950720ULL, 22076131901440ULL, 44152263802880ULL, 88304527605760ULL, 176609055211520ULL, 70643622084608ULL, 565148976676864ULL, 1412872441692160ULL, 2825744883384320ULL, 5651489766768640ULL, 11302979533537280ULL, 22605959067074560ULL, 45211918134149120ULL, 18084767253659648ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};

u64 wPassedPawnMask[] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 217020518514229248ULL, 506381209866534912ULL, 1012762419733069824ULL, 2025524839466139648ULL, 4051049678932279296ULL, 8102099357864558592ULL, 16204198715729117184ULL, 13889313184910671872ULL, 217020518514032640ULL, 506381209866076160ULL, 1012762419732152320ULL, 2025524839464304640ULL, 4051049678928609280ULL, 8102099357857218560ULL, 16204198715714437120ULL, 13889313184898088960ULL, 217020518463700992ULL, 506381209748635648ULL, 1012762419497271296ULL, 2025524838994542592ULL, 4051049677989085184ULL, 8102099355978170368ULL, 16204198711956340736ULL, 13889313181676863488ULL, 217020505578799104ULL, 506381179683864576ULL, 1012762359367729152ULL, 2025524718735458304ULL, 4051049437470916608ULL, 8102098874941833216ULL, 16204197749883666432ULL, 13889312357043142656ULL, 217017207043915776ULL, 506373483102470144ULL, 1012746966204940288ULL, 2025493932409880576ULL, 4050987864819761152ULL, 8101975729639522304ULL, 16203951459279044608ULL, 13889101250810609664ULL, 216172782113783808ULL, 504403158265495552ULL, 1008806316530991104ULL, 2017612633061982208ULL, 4035225266123964416ULL, 8070450532247928832ULL, 16140901064495857664ULL, 13835058055282163712ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};
u64 bPassedPawnMask[] = {0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 3ULL, 7ULL, 14ULL, 28ULL, 56ULL, 112ULL, 224ULL, 192ULL, 771ULL, 1799ULL, 3598ULL, 7196ULL, 14392ULL, 28784ULL, 57568ULL, 49344ULL, 197379ULL, 460551ULL, 921102ULL, 1842204ULL, 3684408ULL, 7368816ULL, 14737632ULL, 12632256ULL, 50529027ULL, 117901063ULL, 235802126ULL, 471604252ULL, 943208504ULL, 1886417008ULL, 3772834016ULL, 3233857728ULL, 12935430915ULL, 30182672135ULL, 60365344270ULL, 120730688540ULL, 241461377080ULL, 482922754160ULL, 965845508320ULL, 827867578560ULL, 3311470314243ULL, 7726764066567ULL, 15453528133134ULL, 30907056266268ULL, 61814112532536ULL, 123628225065072ULL, 247256450130144ULL, 211934100111552ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL};

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

static inline int sideEval(
	const bitboard* const board, 
	bool tomove, 
	const u64 pieces,
	const int pawncount)
{
	const int offset = (tomove == white ? 0 : bking);
	
	int eval = 0;
	const bool opening = (pieces >= OPENING_PIECE_COUNT), 
			middlegame = (pieces >= MIDDLE_PIECE_COUNT);

	static const int connectedBonus[8] = {0, 2, 4, 6, 12, 20, 35, 0};

	u64 curr = board->piece[wpawn + offset];
	while (curr) {
		int square = __builtin_ctzll(curr);
		curr &= curr - 1;
		int i = tableRow(square, tomove), j = square % 8;

		if (white == tomove){
			if (board->piece[wpawn] & wPawnNeighborBonus[square]){
				eval += connectedBonus[i];	
			}
		}
		else {
			if (board->piece[bpawn] & bPawnNeighborBonus[square]){
				eval += connectedBonus[i];
			}
		}

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

static inline int squareColor(int sq) {
    return ((sq >> 3) ^ (sq & 7)) & 1;
}

int staticEval(bitboard *board, bool tomove)
{
	const int pieces = countPieces(board);
	const int pawncount = __builtin_popcountll((board->piece[wpawn] | board->piece[bpawn]));

	if (0 == pawncount){
		// if only knight or bishop left, draw
		if (1 == pieces && ((board->piece[wknight] | board->piece[bknight])
			|| (board->piece[wbishop] | board->piece[bbishop])	
		)){
			return 0;
		}

		// both KNN-K and KN-KN are likely draws
		if (2 == pieces && __builtin_popcountll((board->piece[wknight] | board->piece[bknight])) == 2){
			return 0;
		}

		// Bare kings
		if (pieces == 0)
    		return 0;  
	} 
	
    int score = sideEval(board, white, pieces, pawncount) - sideEval(board, black, pieces, pawncount);

	/* KR vs KB, 
	 * KR vs KN,
	 * KRB vs KR, 
	 * KRN vs KR, 
	 * KNN vs KR 
	 * endgames are often draws. This means, if the advantage is less than a rook, 
	 * and there are no pawns left, the advantage is scaled down. */
	if (0 == pawncount){
		if (2 == pieces){
			if (board->piece[wrook]){
				if (board->piece[bbishop] | board->piece[bknight]){
					score /= 8;	
				}
			}
			else if (board->piece[brook]){
				if (board->piece[wbishop] | board->piece[wknight]){
					score /= 8;	
				}
			}
		}
		if (pieces == 3 && !(board->piece[wqueen] | board->piece[bqueen])) {
			int wr = __builtin_popcountll(board->piece[wrook]);
			int br = __builtin_popcountll(board->piece[brook]);
			int wb = __builtin_popcountll(board->piece[wbishop]);
			int bb = __builtin_popcountll(board->piece[bbishop]);
			int wn = __builtin_popcountll(board->piece[wknight]);
			int bn = __builtin_popcountll(board->piece[bknight]);

			// KR + one minor vs KR
			bool rookMinorVsRook =
				wr == 1 && br == 1 &&
				wb + bb + wn + bn == 1;

			// KNN vs KR, either color
			bool twoKnightsVsRook =
				(wn == 2 && br == 1) ||
				(bn == 2 && wr == 1);

			if (rookMinorVsRook || twoKnightsVsRook){
				score /= 8;
			}
		}
	}

	/* Opposite bishop endgames are often drawish
	 */
	// if (pieces == 2 && abs(score) < 200 && 
	// 	__builtin_popcountll(board->piece[wbishop]) == 1 &&
	// 	__builtin_popcountll(board->piece[bbishop]) == 1)
	// {
	// 	int w = __builtin_ctzll(board->piece[wbishop]);
	// 	int b = __builtin_ctzll(board->piece[bbishop]);

	// 	if (squareColor(w) != squareColor(b))
	// 		score /= 2;
	// }
	
    return tomove == white ? score : -score;
	
	// printBitBoard2d(stdout, *board);
}