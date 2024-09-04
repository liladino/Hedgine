#include "engine.h"

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


#define benchdepth 4
int endcount;

int benchTest(bitboard board, bool tomove, int depth, int maxd){	
	movearray legalmoves;
	
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
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
			if (legalmoves.boards[i].hashValue != hashPosition(&legalmoves.boards[i], !tomove)){
				printf(BG_WHITE TXT_RED "Conversion error" DEFAULT "\n\a");
			}
			else{
				printf(BG_WHITE TXT_GREEN "Conversion correct" DEFAULT "\n");
			}
			printBitPiece(legalmoves.boards[i].hashValue);
			printBitPiece(hashPosition(&legalmoves.boards[i], !tomove));
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

static int countFriendlyPieces(bitboard board, bool tomove){
	u64 all;
	if (white == tomove) all = (board.piece[wqueen] | board.piece[wbishop] | board.piece[wknight] | board.piece[wpawn]);
	else all = (board.piece[bpawn] | board.piece[bqueen] | board.piece[bbishop] | board.piece[bknight] | board.piece[brook]);
	
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
	const int friendlypieces = countFriendlyPieces(board, tomove);
	
	for (int i = (tomove == white ? 0 : 7); (tomove == white ? i < 8 : i >= 0); i += (tomove == white ? 1 : -1)){
		for (int j = 0; j < 8; j++){
			if (board.piece[wpawn + offset] & piecemask){
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
			else if (board.piece[wknight + offset] & piecemask){
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
			else if (board.piece[wbishop + offset] & piecemask){
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
			else if (board.piece[wrook + offset] & piecemask){
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
			else if (board.piece[wking + offset] & piecemask){
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
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	
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

//~ u64 BESTLINEposHashes[absoluteMaxDepth + 1];
//~ move BESTLINEmoves[absoluteMaxDepth];
//~ u64 CurrentlineposHashes[absoluteMaxDepth + 1];

move PV     [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];
u64  PVhash [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];

//~ move nextm; //next move
u64 nextp; //next position
int maxdepth;
int absoluteMaxDepth = MAXSEARCHDEPTH;

bool stopSearch = false;

static int capturesearch(bitboard board, bool tomove, int alpha, int beta){
	int eval = fulleval(board, tomove);
	if (eval >= beta) return beta;
	alpha = max(alpha, eval);
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, true);
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

#ifdef DEBUG
void printLegalmoves(movearray legalmoves, bitboard board, bool tomove){
	printf("\nLegal moves:\n");
	for (int i = 0; i < legalmoves.size; i++){
		printmove(boardConvertTomove(board, legalmoves.boards[i], tomove));
		printHashEntry(legalmoves.boards[i].hashValue);
		//~ printf("%lf\n", legalmoves.boards[i].eval * 0.01);
	}
}
#endif

void communicate() {
	// if time is up break here
	if(info.timeControl == true && getTime_ms()-info.startTime > info.moveTime) {
		// tell engine to stop calculating
		stopSearch = true;
	}
	
	// read GUI input
	readInput();
}

void setMoveTime(){
	if (info.timeRemaining <= 500){ //half a sec left
		info.moveTime = 50;
		return;
	}
	if (info.timeRemaining <= 1000){ //1 sec left
		info.moveTime = 100;
		return;
	}
	if (info.timeRemaining <= 2000){ //2 sec left
		info.moveTime = info.timeRemaining / 5; //gets down from .4 sec to .1 sec
		return;
	}
	if (info.timeRemaining <= 10 * 1000){ //10 sec left
		info.moveTime = 500 + (info.timeRemaining - 2000) / 16; //1 sec and gets down to half sec
		return;
	}
	if (info.timeRemaining <= 30 * 1000){ //30 sec left
		info.moveTime = 1500; //1.5 sec
		return;
	}
	if (info.timeRemaining <= 60 * 1000){ //1 min left
		info.moveTime = 2000; //2 sec
		return;
	}
	if (info.timeRemaining <= 2 * 60 * 1000){ //2 min left
		info.moveTime = 3000; //3 sec
		return;
	}
	if (info.timeRemaining <= 3 * 60 * 1000){ //3 min left
		info.moveTime = 4000; //4 sec
		return;
	}
	if (info.timeRemaining <= 5 * 60 * 1000){ //5 min left
		info.moveTime = 6000; 
		return;
	}
	if (info.timeRemaining <= 10 * 60 * 1000){ //10 min left
		info.moveTime = 8000; 
		return;
	}
	if (info.timeRemaining <= 20 * 60 * 1000){ //20 min left
		info.moveTime = 15000; 
		return;
	}
	if (info.timeRemaining <= 45 * 60 * 1000){ //45 min left
		info.moveTime = 30000; 
		return;
	}
	info.moveTime = 40000; 
}

unsigned int searchedNodes;

int search(bitboard board, bool tomove, int depth, int alpha, int beta){
	searchedNodes++;
	if (searchedNodes % 2011 == 0){
		communicate();
	}	
	if (stopSearch) {
		return 0;
	}
	
	const int oddity = depth % 2;
	evalflag flag = alphaFlag;
	
	int eval = NO_HASH_ENTRY;
	
	//have to rework the PV system, the TT isn't much help this way
	
	if (PVhash[0][depth] != board.hashValue){
		//~ eval = readHashEntry(board.hashValue, &alpha, &beta, depth, maxdepth, oddity);
		if (eval != NO_HASH_ENTRY){
			move nullmove = {{-1, -1}, {-1, -1}, 0};
			for (int i = depth; i <= maxdepth; i++){
				PV[depth][i] = nullmove;
				PVhash[depth][i+1] = 0;
			}
			return eval;
		}
	}
	
	if (depth == maxdepth){
		return capturesearch(board, tomove, alpha, beta);
	}
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	if (legalmoves.size == 0){
		if (bitInCheck(board, tomove)) return blackwon - 100 + depth;
		return draw;
	}
	
	orderMoves(&legalmoves);
	
	for (int i = 0; i < legalmoves.size; i++){
		eval = -search(legalmoves.boards[i], !tomove, depth+1, -beta, -alpha);
		
		if (stopSearch) {
			for (int i = depth; i < maxdepth; i++){
				PV[depth][i] = PV[depth + 1][i];
				PVhash[depth][i+1] = PVhash[depth + 1][i+1];
			}
			return 0;
		}
		
		if (eval >= beta){
			if (oddity) storePos(board.hashValue, -beta, betaFlag, maxdepth - depth);
			else storePos(board.hashValue, beta, betaFlag, maxdepth - depth);
			return beta;
		}
		if (alpha < eval){ 
			flag = exactFlag;
			alpha = eval;
			//~ bestindex = i;
			if (depth == 0){
				//~ nextm = boardConvertTomove(board, legalmoves.boards[i], tomove);
				//~ PV[1][1] = nextm;
				nextp = legalmoves.boards[i].hashValue;
			}
			PV[depth+1][depth] = boardConvertTomove(board, legalmoves.boards[i], tomove);
			PVhash[depth+1][depth+1] = legalmoves.boards[i].hashValue;
		}
	}
	for (int i = depth; i < maxdepth; i++){
		PV[depth][i] = PV[depth + 1][i];
		PVhash[depth][i+1] = PVhash[depth + 1][i+1];
	}
	
	if (oddity) storePos(board.hashValue, -alpha, flag, maxdepth - depth);
	else storePos(board.hashValue, alpha, flag, maxdepth - depth);
	return alpha;
}

move engine(bitboard board, bool tomove){
	move nullmove = {{-1, -1}, {-1, -1}, 0};
	move nextm = nullmove;
	
	PVhash[0][0] = board.hashValue;
	for (int i = 1; i < MAXSEARCHDEPTH; i++){
		//~ PV[0][i] = nullmove;
		PVhash[0][i] = 0;
	}

	
	stopSearch = false;
	info.startTime = getTime_ms();
	
	int i;
	for (i = 1; i < absoluteMaxDepth + 1; i++){
		for (unsigned j = 0; j < MAXSEARCHDEPTH; j++){
			for (unsigned k = 0; k < MAXSEARCHDEPTH; k++){
				//~ if ((k | j) == 0) continue;
				PV[j][k] = nullmove;
			}
		}
		
		maxdepth = i;
		
		int temp = search(board, tomove, 0, NegINF, PosINF);
		//~ printf("%d\n", temp);
		
		storePos(nextp, temp, lastBest, maxdepth);
		
		#ifdef DEBUG
		printf("depth %d\n", i);
		movearray legalmoves;
		bitGenerateLegalmoves(&legalmoves, board, tomove, false);		
		orderMoves(&legalmoves);
		printmove(boardConvertTomove(board, legalmoves.boards[0], tomove));
		printHashEntry(legalmoves.boards[0].hashValue);	
		#endif
		
		for (int j = 0; j < i && PV[0][j].from.rank != -1 && !stopSearch; j++){
			printmove(PV[0][j]);
		}
		printf("\n");
		
		
		if (PV[0][0].from.rank != -1) nextm = PV[0][0];
		
		if (stopSearch){
			#ifdef DEBUG
			printf("stopped\n\n");
			#endif
			break;
		} 
		
		if (temp >= whitewon || temp <= blackwon) break; //dont think if not neccesary
	}
	
	rmBestMoveFlag(nextp);
	
	#ifdef DEBUG
	printCollisionStats();
	printf("\n\n");
	#endif 
	
	
	//maxdepth = 4;
	/*search(board, tomove, maxdepth, NegINF, PosINF);
	
	u64 temp;
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, &temp, false);
	
	printLegalmoves(legalmoves, board, tomove);
	printLegalmoves(legalmoves, board, tomove);*/
	
	
	
	return nextm;
}


/* felhasznalo kivalasztja, hogy milyen erossegu sakkmotorral gondolkozzon a 
 * program, es ez visszaad a megfelelo erosseggel egy lepest*/
move CPU(int cpulvl, bitboard bboard, bool tomove){
	squarenums start = {-1, -1};
	move m = initializemove(start, start, 0);
	//~ bitboard bboard = boardConvert(board, castling, enpass, tomove);

	//Engine depth should be determined by the maximum depth it can reach or the thinking time
	//i'm thinking max depth is slightly more reasonable
	
	if (cpulvl == 0){
		usleep(millisec * 50);//hogy ne azonnal lepjen, nem letszukseglet
		m = randomBot(bboard, tomove);
	}	
	else{
		absoluteMaxDepth = (cpulvl > 40 ? 40 : cpulvl);
		m = engine(bboard, tomove);	
	}
	return m;
}
