#include "bitengine.h"

void printLogo(){
	printf("                                     /    #\n .  .     .                       #   *.  # #   ##  .#\n |__| _  _| _ . _  _       # (  #       #.   #,     /,##,     #(\n |  |(/,(_](_]|[ )(/,  (  #   ##                             # .,\n           ._|        # #.,                                      #\n                     #   #                                              /#\n                  ./##                           ||                  *#\n                   #                            ====                     ##\n                   #                       ____  ||  ____                  #\n                  +.    &&                / __ \\ /\\ / __ \\                /#\n                 &   &&     &&           | /  \\ |  | /  \\ |               #\n     &&*       &    *&& #      &         | \\   \\ \\/ /   / |                ,.\n   &    &#+&&     .#    &       &         \\ \\__/ || \\__/ /                (,\n   &     &         &  & &        &        |______________|                #*#\n    &&&&.           /&&          &         \\____________/                #\n        &                       /.                                 (###/  #\n         &        # &           &                                   (\n           *&      &           &                        .##    #   ###\n               *&&          *&#  ,##   #    ((##( /   ##  #  #*#\n                                     (#    ###     \n");	
}

static int max(int a, int b){
	return (a > b ? a : b);
}

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
	{  10,  40,  10,  40,  40, -10,   0,  10}, //3
	{   0, -30,  50,  50,  40,  10, -30,   0}, //4
	{   0,   0,   0,  45,  45,   0,   0,   0}, //5
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //6
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //7
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //8
};

int middlepawn[8][8] = {
	{   0,   0,   0,   0,   0,   0,   0,   0}, //1
	{   0,   0,   0, -20, -20,  10,   0,   0}, //2
	{  10,  10,  10,  20,  20, -20,   0,  10}, //3
	{   0, -10,  40,  50,  50, -20, -10,   0}, //4
	{  10,  10,  20,  50,  50,  10,  10,  10}, //5
	{ 100, 100, 100, 100, 100, 100, 100, 100}, //6
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //7
	{ 200, 200, 200, 200, 200, 200, 200, 200}, //8
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
	{  -5,  -5,  -3,  -1,  -1,  -3,  -5,  -5}, //1
	{  -5,   3,   4,   5,   5,   4,   3,  -5}, //2
	{  -3,   4,   9,  10,  10,   9,   4,  -3}, //3
	{  -1,   5,  10,  13,  13,  10,   5,  -1}, //4
	{  -1,   5,  10,  13,  13,  10,   5,  -1}, //5
	{  -3,   4,   9,  10,  10,   9,   4,  -3}, //6
	{  -5,   3,   4,   5,   5,   4,   3,  -5}, //7
	{  -5,  -5,  -3,  -1,  -1,  -3,  -5,  -5}, //8
};


#define benchdepth 4
int endcount;

int benchTest(bitboard board, bool tomove, int depth, int maxd){	
	u64 attackedsquares = 0;
	move_array legalmoves;
	
	bitGenerateLegalmoves(&legalmoves, board, tomove, &attackedsquares, false);
	if (legalmoves.size == 0){
		endcount++;
	}
	if (depth == 0){
		return legalmoves.size;
	}
	int all = 0;
	for (int i = 0; i < legalmoves.size; i++){
		int x = benchTest(legalmoves.boards[i], !tomove, depth-1, maxd);
		all += x;
		#ifdef DEBUG
		if (maxd == 1){
			printBitBoard2d(legalmoves.boards[i]);
			if (legalmoves.boards[i].hashValue != hashPosition(legalmoves.boards[i], !tomove)){
				printf(BG_WHITE TXT_RED "Conversion error" DEFAULT "\n\a");
			}
			else{
				printf(BG_WHITE TXT_GREEN "Conversion correct" DEFAULT "\n");
			}
			printBitPiece(legalmoves.boards[i].hashValue);
			printBitPiece(hashPosition(legalmoves.boards[i], !tomove));
		}
		#endif
		if (maxd == benchdepth && maxd == depth){
			printmove(boardConvertTomove(board, legalmoves.boards[i], tomove));
			printf(": %d\n", x);
		}
	}
	return all;
}

void makeBenchTest(char board[12][12], bool tomove, int castling[4], squarenums enpass){
	printLogo();
	endcount = 0;
	bitboard bboard = boardConvert(board, castling, enpass, tomove);
	for (int i = 1; i <= benchdepth; i++){
		endcount = 0;
		int allmovecount = benchTest(bboard, tomove, i, i);
		printf("%d ply:\nAll leaves: %d\nGame ends (total): %d\n\n", i, allmovecount, endcount);
	}
}

static int countPieces(bitboard board){
	u64 all = (board.piece[wqueen] | board.piece[wbishop] | board.piece[wknight] | board.piece[wrook] | board.piece[bqueen] | board.piece[bbishop] | board.piece[bknight] | board.piece[brook]);
	u64 mask = 1;
	int count = 0;
	for (int i = 0; i < 64; i++){
		if (mask & all) count++;
		mask = mask << 1;
	}
	return count;
}

static int sideEval(bitboard board, bool tomove){
	int offset = (tomove == white ? 0 : bking);
	u64 piecemask = 1;
	int eval = 0;
	const int pieces = countPieces(board);
	
	for (int i = (tomove == white ? 0 : 7); (tomove == white ? i < 8 : i >= 0); i += (tomove == white ? 1 : -1)){
		for (int j = 0; j < 8; j++){
			if (board.piece[wpawn + offset] & piecemask){
				eval += 100;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingpawn[i][j] * 0.2 + (14 - pieces) * middlepawn[i][j] * 0.2);
				}
				else {
					eval += middlepawn[i][j] + 10;
				}
			}
			else if (board.piece[wknight + offset] & piecemask){
				eval += 310;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingknight[i][j] * 0.2 + (14 - pieces) * middleknight[i][j] * 0.2) + 15;
				}
				else if (pieces > 5){
					eval += middleknight[i][j];
				}
				else {
					eval += ((pieces - 1) * middleknight[i][j] * 0.2 + (5 - pieces) * endgamecenter[i][j] * 0.5);
				}
			}
			else if (board.piece[wbishop + offset] & piecemask){
				eval += 313;
				if (pieces > 8) {
					eval += ((pieces - 9) * openingbishop[i][j] * 0.2 + (14 - pieces) * middlebishop[i][j] * 0.2);
				}
				else if (pieces > 5){
					eval += middlebishop[i][j] + 15;
				}
				else {
					eval += ((pieces - 1) * middlebishop[i][j] * 0.2 + (5 - pieces) * endgamecenter[i][j] * 0.5) + 20;
				}
			}
			else if (board.piece[wrook + offset] & piecemask){
				eval += 500;
				if (pieces > 8) {
					eval += ((pieces - 9) * 500 * 0.2 + (14 - pieces) * middlerook[i][j] * 0.2) - 10;
				}
				else if (pieces > 6){
					eval += middlerook[i][j];
				}
				else {
					eval += ((pieces - 1) * middlerook[i][j] * 0.2 + (6 - pieces) * endgamecenter[i][j] * 0.5);
				}
			}
			else if (board.piece[wking + offset] & piecemask){
				if (pieces > 8) {
					eval += ((pieces - 9) * openingking[i][j] * 0.2 + (14 - pieces) * middleking[i][j] * 0.2) - 10;
				}
				else if (pieces > 6){
					eval += middleking[i][j];
				}
				else {
					eval += (max(pieces - 1, 0) * middleking[i][j] * 0.2 + (6 - pieces) * endgamecenter[i][j]);
				}
			}
			else if (board.piece[wqueen + offset] & piecemask){
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

static int fulleval(bitboard board, bool tomove){
	resultconst r = gameend(board, tomove);
	if (r != ongoing) return r * (tomove == white ? 1 : -1);
	return (sideEval(board, white) + sideEval(board, black)) * (tomove == white ? 1 : -1);
}

move randomBot(bitboard board, bool tomove){
	#ifdef DEBUG
	printf("w_eval: %d\n", sideEval(board, white));
	printf("b_eval: %d\n", sideEval(board, black));
	#endif
	time_t t;
	srand((unsigned) time (&t));
	move_array legalmoves;
	u64 temp = 0;
	bitGenerateLegalmoves(&legalmoves, board, tomove, &temp, false);
	int i = (rand() % legalmoves.size);
	move m = boardConvertTomove(board, legalmoves.boards[i], tomove);
	#ifdef DEBUG
	printf("%dth move: \n", i);
	printmove(m);
	printf("\n");
	printBitBoard(board);
	printf("w_eval: %d\n", sideEval(legalmoves.boards[i], white));
	printf("b_eval: %d\n", sideEval(legalmoves.boards[i], black));
	#endif
	return m;
}


bitboard next;
move nextm;

int maxdepth;

int capturesearch(bitboard board, bool tomove, int alpha, int beta){
	int eval = fulleval(board, tomove);
	if (eval >= beta) return beta;
	alpha = max(alpha, eval);
	
	move_array legalmoves;
	u64 attackedsquares = 0;
	bitGenerateLegalmoves(&legalmoves, board, tomove, &attackedsquares, true);
	if (legalmoves.size == 0){
		return eval;
	}
	
	for (int i = 0; i < legalmoves.size; i++){
		int eval = -capturesearch(legalmoves.boards[i], !tomove, -beta, -alpha);
		if (eval >= beta){
			return beta;
		}
		alpha = max(alpha, eval);
	}
	return alpha;
}

int search(bitboard board, bool tomove, int depth, int alpha, int beta){
	const int oddity = (maxdepth - depth) % 2;
	evalflag flag = alphaFlag;
	
	int evalFromHash;
	if (oddity) evalFromHash = -readHashEntry(board.hashValue, -beta, -alpha, depth);
	else evalFromHash = readHashEntry(board.hashValue, alpha, beta, depth);
	
	if (evalFromHash != NO_HASH_ENTRY && evalFromHash != -1 * NO_HASH_ENTRY){
		if (depth == maxdepth) nextm = readHashEntryMove(board.hashValue);
		return evalFromHash;
	}
	if (depth == 0){
		return capturesearch(board, tomove, alpha, beta);
	}
	
	move_array legalmoves;
	u64 attackedsquares = 0;
	bitGenerateLegalmoves(&legalmoves, board, tomove, &attackedsquares, false);
	if (legalmoves.size == 0){
		if (bitInCheck(board, tomove)) return blackwon - depth;
		return draw;
	}
	move currbestmove = {{-1, -1}, {-1, -1}, 0};
	int bestindex = 0;
	for (int i = 0; i < legalmoves.size; i++){
		int eval = -search(legalmoves.boards[i], !tomove, depth-1, -beta, -alpha);
		if (eval >= beta){
			if (oddity) storePos(board.hashValue, -beta, betaFlag, depth, currbestmove, legalmoves.boards[bestindex].hashValue);
			else storePos(board.hashValue, beta, betaFlag, depth, currbestmove, legalmoves.boards[bestindex].hashValue);
			return beta;
		}
		if (alpha < eval){ 
			flag = exactFlag;
			alpha = eval;
			bestindex = i;
			if (depth == maxdepth){
				nextm = boardConvertTomove(board, legalmoves.boards[i], tomove);
				currbestmove = nextm;
			}
			else{
				currbestmove = boardConvertTomove(board, legalmoves.boards[i], tomove);
			}
		}
	}
	if (oddity) storePos(board.hashValue, -alpha, flag, depth, currbestmove, legalmoves.boards[bestindex].hashValue);
	else storePos(board.hashValue, alpha, flag, depth, currbestmove, legalmoves.boards[bestindex].hashValue);
	return alpha;
}

move engine(bitboard board, bool tomove){
	/*
	for (int i = 4; i < 5; i++){
		maxdepth = i;
		clearTransTable();
		search(board, tomove, maxdepth, NegINF, PosINF);
	}*/
	
	maxdepth = 4;
	search(board, tomove, maxdepth, NegINF, PosINF);
	printBestLine(board.hashValue);
	
	return nextm;
}

int badsearch(bitboard board, bool tomove){
	int worsteval = PosINF;
	
	move_array legalmoves;
	u64 attackedsquares = 0;
	bitGenerateLegalmoves(&legalmoves, board, tomove, &attackedsquares, false);
	
	if (legalmoves.size == 0){//technically cannot reach this
		return draw;
	}
	
	for (int i = 0; i < legalmoves.size; i++){
		int eval = -fulleval(legalmoves.boards[i], !tomove);
		if (worsteval > eval){ 
			worsteval = eval;
			next = legalmoves.boards[i];
		}
	}
	return worsteval;
}

move loBOTomy(bitboard board, bool tomove){
	//maxdepth = 1;
	badsearch(board, tomove);
	return boardConvertTomove(board, next, tomove);
}

/* felhasznalo kivalasztja, hogy milyen erossegu sakkmotorral gondolkozzon a 
 * program, es ez visszaad a megfelelo erosseggel egy lepest*/
move CPU(int cpulvl, char board[12][12], bool tomove, int castling[4], squarenums enpass){
	squarenums start = {-1, -1};
	move m = initializemove(start, start, 0);
	bitboard bboard = boardConvert(board, castling, enpass, tomove);
	//printBitPiece(bboard.hashValue);
	switch(cpulvl){
		case 0: 
			usleep(millisec * 50);
			m = loBOTomy(bboard, tomove);
			break;
		case 1: 
			usleep(millisec * 50);
			m = randomBot(bboard, tomove);
			break;
		default: 
			//usleep(millisec * 50);
			m = engine(bboard, tomove);		
		
	}
	return m;
}
