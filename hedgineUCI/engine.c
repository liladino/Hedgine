#include "engine.h"

void printLogo(){
	printf("                                     /    #\n .  .     .                       #   *.  # #   ##  .#\n |__| _  _| _ . _  _       # (  #       #.   #,     /,##,     #(\n |  |(/,(_](_]|[ )(/,  (  #   ##                             # .,\n           ._|        # #.,                                      #\n                     #   #                                              /#\n                  ./##                           ||                  *#\n                   #                            ====                     ##\n                   #                       ____  ||  ____                  #\n                  +.    &&                / __ \\ /\\ / __ \\                /#\n                 &   &&     &&           | /  \\ |  | /  \\ |               #\n     &&*       &    *&& #      &         | \\   \\ \\/ /   / |                ,.\n   &    &#+&&     .#    &       &         \\ \\__/ || \\__/ /                (,\n   &     &         &  & &        &        |______________|                #*#\n    &&&&.           /&&          &         \\____________/                #\n        &                       /.                                 (###/  #\n         &        # &           &                                   (\n           *&      &           &                        .##    #   ###\n               *&&          *&#  ,##   #    ((##( /   ##  #  #*#\n                                     (#    ###     \n");	
}

static int max(int a, int b){
	return (a > b ? a : b);
}

static int absint(int a){
	return (a < 0 ? -a : a);
}

/*
 * BENCHMARK
 * */

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
			printmove(boardConvertTomove(&board, &legalmoves.boards[i], tomove));
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

/*
 * UCI comunication
 * */

void communicate() {
	// if time is up break here
	if(info.timeControl == true && getTime_ms()-info.startTime > info.moveTime) {
		// tell engine to stop calculating
		stopSearch = true;
	}
	
	// read GUI input
	readInput();
}

void setMoveTime(int increment){
	info.moveTime = increment;
	if (info.timeRemaining <= 500){ //half a sec left
		info.moveTime += 50;
		return;
	}
	if (info.timeRemaining <= 1000){ //1 sec left
		info.moveTime += 100;
		return;
	}
	if (info.timeRemaining <= 2000){ //2 sec left
		info.moveTime += info.timeRemaining / 5; //gets down from .4 sec to .1 sec
		return;
	}
	if (info.timeRemaining <= 10 * 1000){ //10 sec left
		info.moveTime += 500 + (info.timeRemaining - 2000) / 16; //1 sec and gets down to half sec
		return;
	}
	if (info.timeRemaining <= 30 * 1000){ //30 sec left
		info.moveTime += 1500; //1.5 sec
		return;
	}
	if (info.timeRemaining <= 60 * 1000){ //1 min left
		info.moveTime += 2100; //2.1 sec
		return;
	}
	if (info.timeRemaining <= 2 * 60 * 1000){ //2 min left
		info.moveTime += 3720; //3.7 sec
		return;
	}
	if (info.timeRemaining <= 3 * 60 * 1000){ //3 min left
		info.moveTime += 5450; //5.5 sec
		return;
	}
	if (info.timeRemaining <= 5 * 60 * 1000){ //5 min left
		info.moveTime += 7910; 
		return;
	}
	if (info.timeRemaining <= 10 * 60 * 1000){ //10 min left
		info.moveTime += 9900; 
		return;
	}
	if (info.timeRemaining <= 20 * 60 * 1000){ //20 min left
		info.moveTime += 15000; 
		return;
	}
	if (info.timeRemaining <= 45 * 60 * 1000){ //45 min left
		info.moveTime += 30000; 
		return;
	}
	info.moveTime += 40000; 	
}

/* 
 * RANDOM MOVER BOT
 *  */

move randomBot(bitboard board, bool tomove){
	time_t t;
	srand((unsigned) time (&t));
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	
	int i = (rand() % legalmoves.size);
	move m = boardConvertTomove(&board, &legalmoves.boards[i], tomove);
	
	return m;
}

move PV     [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];

u64 nextp; //next position
int maxdepth;
int absoluteMaxDepth = MAXSEARCHDEPTH;

bool stopSearch = false;

static int quiescenceSearch(bitboard board, bool tomove, int alpha, int beta){
	int eval = fulleval(&board, tomove);
	if (eval >= beta) return beta;
	alpha = max(alpha, eval);
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, true);
	if (legalmoves.size == 0){
		return eval;
	}
	
	for (int i = 0; i < legalmoves.size; i++){
		int eval = -quiescenceSearch(legalmoves.boards[i], !tomove, -beta, -alpha);
		if (eval >= beta){
			return beta;
		}
		alpha = max(alpha, eval);
	}
	return alpha;
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
	
	//~ if (depth != 0 && isRepetition(board.hashValue)) return 0;
	
	const int oddity = depth % 2;
	evalflag flag = alphaFlag;
	
	int eval = NO_HASH_ENTRY;
	
	bool PVnode = (beta - alpha > 1);
	
	if (!PVnode && searchedNodes > 1){
		//~ eval = readHashEntry(board.hashValue, &alpha, &beta, depth, maxdepth, oddity);
		if (eval != NO_HASH_ENTRY){
			move nullmove = {{-1, -1}, {-1, -1}, 0};
			for (int i = depth; i <= maxdepth; i++){
				PV[depth][i] = nullmove;
			}
			return eval;
		}
	}
	
	if (depth >= maxdepth){
		return quiescenceSearch(board, tomove, alpha, beta);
	}
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	if (legalmoves.size == 0){
		if (bitInCheck(board, tomove)) return blackwon - 100 + depth;
		return draw;
	}
	
	orderMoves(&legalmoves);
		
	for (int i = 0; i < legalmoves.size; i++){
		storeRepetiton(board.hashValue);
		if (i == 0 || maxdepth < 2){
			eval = -search(legalmoves.boards[i], !tomove, depth+1, -beta, -alpha);
		}
		else {
			eval = -search(legalmoves.boards[i], !tomove, depth+1, -alpha-1, -alpha);
			if (eval > alpha && eval < beta){
				eval = -search(legalmoves.boards[i], !tomove, depth+1, -beta, -alpha);
			}
		}
		rmLastRepetition();
		
		if (eval >= beta){
			if (oddity) storePosTT(board.hashValue, -beta, betaFlag, maxdepth - depth);
			else storePosTT(board.hashValue, beta, betaFlag, maxdepth - depth);
			
			return beta;
		}
		if (alpha < eval){ 
			flag = exactFlag;
			alpha = eval;
			//~ bestindex = i;
			if (depth == 0){
				nextp = legalmoves.boards[i].hashValue;
			}
			PV[depth+1][depth] = boardConvertTomove(&board, &legalmoves.boards[i], tomove);
		}
		
		if (stopSearch) {
			for (int i = depth; i < maxdepth; i++){
				PV[depth][i] = PV[depth + 1][i];
			}
			
			return 0;
		}
	}
	
	for (int i = depth; i < maxdepth; i++){
		PV[depth][i] = PV[depth + 1][i];
	}
	
	if (oddity) storePosTT(board.hashValue, -alpha, flag, maxdepth - depth);
	else storePosTT(board.hashValue, alpha, flag, maxdepth - depth);
	return alpha;
}

move engine(bitboard board, bool tomove){
	searchedNodes = 0;
	move nullmove = {{-1, -1}, {-1, -1}, 0};
	move nextm = nullmove;
	#ifdef DEBUG
	printf("debug\tthinking time %d\n", info.moveTime);
	#endif
	
	for (int i = 1; i < MAXSEARCHDEPTH; i++){
		PV[0][i] = nullmove;
	}

	
	stopSearch = false;
	info.startTime = getTime_ms();
	int temp = 0;
	int i;
	for (i = 1; i < absoluteMaxDepth + 1; i++){
		for (unsigned j = 0; j < MAXSEARCHDEPTH; j++){
			for (unsigned k = 0; k < MAXSEARCHDEPTH; k++){
				//~ if ((k | j) == 0) continue;
				PV[j][k] = nullmove;
			}
		}
		maxdepth = i;
		
		temp = search(board, tomove, 0, NegINF, PosINF);
		
		//store the best move with a special flag to make sure next search starts with it
		storePosTT(nextp, temp, lastBest, maxdepth);
		
		if (PV[0][0].from.rank != -1) nextm = PV[0][0];
		
		if (stopSearch){
			break;
		} 
		
		printf("info depth %d", i);
		
		if (temp >= whitewon || temp <= blackwon){
			printf(" score mate %d pv ", absint(temp) - whitewon + 1);
		}
		else{
			printf(" score cp %d pv ", (tomove == black ? -1 : 1) * temp);
		}
		
		for (int j = 0; j < i && PV[0][j].from.rank != -1 && !stopSearch; j++){
			printmove(PV[0][j]);
		}
		printf("\n");
		
		if (temp >= whitewon || temp <= blackwon) break; //dont think if not neccesary
	}
	
	//~ rmBestMoveFlag(nextp);
	
	//~ #ifdef DEBUG
	//~ printCollisionStats();
	//~ printf("\n\n");
	//~ #endif 
	
	return nextm;
}

/* 
 * CPU
 * recieves a move request, starts the thinking at the adequate strength
 * */
move CPU(int cpulvl, bitboard bboard, bool tomove){
	squarenums start = {-1, -1};
	move m = initializemove(start, start, 0);
	//~ bitboard bboard = boardConvert(board, castling, enpass, tomove);

	//Engine depth should be determined by the maximum depth it can reach or the thinking time
	//i'm thinking max depth is slightly more reasonable
	
	if (cpulvl < 0) cpulvl = 0;
	if (cpulvl > 39) cpulvl = 39;
	
	if (cpulvl == 0){
		usleep(millisec * 50);//hogy ne azonnal lepjen, nem letszukseglet
		m = randomBot(bboard, tomove);
	}	
	else{
		absoluteMaxDepth = cpulvl;
		m = engine(bboard, tomove);	
	}
	return m;
}
