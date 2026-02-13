#include "headers/engine.h"

/*
 * CONSTANTS 
 * */
const move    NULLMOVE = {{-1, -1}, {-1, -1}, 0};
const bitMove NULLBITMOVE = {0, 0, 0, 0, 0};
#define PosINF   2147480000  //less than 2^31 to avoid possible overflow
#define NegINF  -2147480000
#define MATE_NOW WHITEWON
#define MAXSEARCHDEPTH 40

/*
 * STATICS & GLOBALS
 * */
static bitMove      s_PVtable [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];
static bitMove      s_nextmove;
static int          s_maxdepth;
static int          s_absoluteMaxDepth = MAXSEARCHDEPTH;
static unsigned int s_searchedNodes;
static bitboard     s_searchBoard; //the search takes place on this
       bool         g_stopSearch = false;

/*
 * GENERAL HELPER FUNCS
 * */
static inline int i_max(int a, int b){
	return (a > b ? a : b);
}

static inline int i_min(int a, int b){
	return (a < b ? a : b);
}

static inline int i_abs(int a){
	return (a < 0 ? -a : a);
}

bool isMateScore(int score) {
	return i_abs(score) > MATE_NOW - MAXSEARCHDEPTH;
}

/*
 * UCI comunication
 * */
void communicate() {
	if(info.timeControl == true && getTime_ms()-info.startTime > info.moveTime) {
		g_stopSearch = true;
	}
	
	readInput();
}

/* 
 * RANDOM MOVER BOT
 *  */
move randomBot(bitboard board, bool tomove){
	time_t t;
	srand((unsigned) time (&t));
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, &board, tomove, false);
	
	int i = (rand() % legalmoves.size); 
	return convertBitMoveToMove(legalmoves.array[i]);
}

static int quiescenceSearch(bool tomove, int alpha, int beta){
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }
	
	int eval = fulleval(&s_searchBoard, tomove, s_maxdepth);
	if (eval >= beta) return beta;
	alpha = i_max(alpha, eval);
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, &s_searchBoard, tomove, true);
	if (legalmoves.size == 0){
		return eval;
	}
	
	for (int i = 0; i < legalmoves.size; i++) {
		//check if legal at all
		if (!isCastlingLegal(&s_searchBoard, tomove, &legalmoves.array[i])){
			continue;
		}
		bitUndo u = makeMove(&s_searchBoard, legalmoves.array[i]);
		if (bitInCheck(&s_searchBoard, tomove)){
			undoMove(&s_searchBoard, legalmoves.array[i], u);
			continue;
		}
		//search
		int eval = -quiescenceSearch(!tomove, -beta, -alpha);
		undoMove(&s_searchBoard, legalmoves.array[i], u);
		
		if (eval >= beta){
			return beta;
		}
		alpha = i_max(alpha, eval);
	}
	return alpha;
}

/* depth: from the root node, 0: nothing searched yet
 * remainingDepth: decreasing to 0
 * 
 * Credits partially to Sebastian Lague
 * */
int search(bool tomove, int remainingDepth, int depth, int alpha, int beta){
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }
	
	if (0 < depth) {
		if (isRepetition(s_searchBoard.hashValue)) return 0;
		
		alpha = i_max(alpha, -MATE_NOW + depth);
		beta  = i_min(beta,   MATE_NOW - depth);
		if (alpha >= beta) {
			return alpha;
		}
	}
	
	int eval = readHashEntry(s_searchBoard.hashValue, remainingDepth, depth, alpha, beta);
	if (eval != NO_HASH_ENTRY){
		for (int i = depth; i <= s_maxdepth; i++){
			//do i really need this?
			s_PVtable[depth][i] = NULLBITMOVE;
		}
		return eval;
	}
	
	if (0 == remainingDepth) {
		return quiescenceSearch(tomove, alpha, beta);
	}

	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, &s_searchBoard, tomove, false);
	if (0 == legalmoves.size){
		if (bitInCheck(&s_searchBoard, tomove)) {
			return -(MATE_NOW - depth);
		}
		else {
			return 0;
		}
	}
	
	/*
	 * Order moves
	 * */
	
	//not done yet
	
	/*
	 * Store repetition
	 * */
	storeRepetiton(s_searchBoard.hashValue);
	
	//~ int evaluationBound = TranspositionTable.UpperBound;
	//~ Move bestMoveInThisPosition = Move.NullMove;
	evalflag moveFlag = UPPER_BOUND_FLAG;
	bitMove* bestMove = &legalmoves.array[0];

	for (int i = 0; i < legalmoves.size; i++) { 
		bitMove* currentMove = &legalmoves.array[i];
		
		//check if legal at all
		if (!isCastlingLegal(&s_searchBoard, tomove, currentMove)){
			continue;
		}
		bitUndo u = makeMove(&s_searchBoard, *currentMove);
		if (bitInCheck(&s_searchBoard, tomove)){
			undoMove(&s_searchBoard, *currentMove, u);
			continue;
		}
		
		/*
		 * Extension?
		 * */
		
		//~ TODO
		
		/*
		 * Reduce?
		 * (not interesting moves, e.g. late in ordering)
		 * */
		 
		//~ TODO
		
		/*
		 * Search
		 * */
		int eval = search(!tomove, remainingDepth-1, depth+1, -beta, -alpha);
		undoMove(&s_searchBoard, *currentMove, u);
		
		/*
		 * Pruning
		 * */
		if (eval >= beta) {
			storePosTT(s_searchBoard.hashValue, beta, LOWER_BOUND_FLAG, remainingDepth, depth, currentMove);
						
			//~ transpositionTable.StoreEvaluation(plyRemaining, plyFromRoot, beta, TranspositionTable.LowerBound, moves[i]);

			/*
			 * Killer move history
			 * */
			//~ if (!(currentMove->flags & CAPTURE_FLAG)) {
				//~ if (plyFromRoot < MoveOrdering.maxKillerMovePly) {
					//~ moveOrderer.killerMoves[plyFromRoot].Add(move);
				//~ }
				//~ int historyScore = plyRemaining * plyRemaining;
				//~ moveOrderer.History[board.MoveColourIndex, moves[i].StartSquare, moves[i].TargetSquare] += historyScore;
			//~ }
			
			
			rmLastRepetition();

			return beta;
		}
		 
		/*
		 * New best move foud
		 * */
		if (eval > alpha) {
			moveFlag = EXACT_EVAL_FLAG;
			//~ bestMoveInThisPosition = moves[i]; -> for TT

			alpha = eval;
			if (0 == depth) {
				s_nextmove = *currentMove;
				//~ bestEvalThisIteration = eval;
			}
		}
	}
		
	rmLastRepetition();
	storePosTT(s_searchBoard.hashValue, alpha, moveFlag, remainingDepth, depth, bestMove);	
	
	//~ transpositionTable.StoreEvaluation(plyRemaining, plyFromRoot, alpha, evaluationBound, bestMoveInThisPosition);

	return alpha;
}

/*
int search(bitboard board, bool tomove, int depth, int alpha, int beta){
	s_searchedNodes++;
	if (s_searchedNodes % 2011 == 2){
		communicate();
	}	
	if (g_stopSearch) {
		return 0;
	}
	
	if (depth > 0 && isRepetition(board.hashValue)) return 0;
	
	const int oddity = depth % 2;
	evalflag flag = LOWER_BOUND_FLAG;
	
	bool PVnode = (beta - alpha > 1);
	
	int eval = NO_HASH_ENTRY;
	
	if (!PVnode && depth > 0 && s_maxdepth > 2){ //don't swearch at low depth  
		eval = readHashEntry(board.hashValue, &alpha, &beta, depth, s_maxdepth, oddity);
		if (eval != NO_HASH_ENTRY){
			for (int i = depth; i <= s_maxdepth; i++){
				s_PVtable[depth][i] = NULLMOVE;
			}
			return eval;
		}
	}
	
	if (depth >= s_maxdepth){
		return quiescenceSearch(board, tomove, alpha, beta);
	}
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	if (legalmoves.size == 0){
		if (bitInCheck(&board, tomove)) return BLACKWON - 100 + depth;
		return DRAW;
	}
	
	orderMoves(&legalmoves);
		
	storeRepetiton(board.hashValue);
	for (int i = 0; i < legalmoves.size; i++){
		if (i == 0 || s_maxdepth < 2){
			eval = -search(legalmoves.boards[i], !tomove, depth+1, -beta, -alpha);
		}
		else {
			eval = -search(legalmoves.boards[i], !tomove, depth+1, -alpha-1, -alpha);
			if (eval > alpha && eval < beta){ 
				eval = -search(legalmoves.boards[i], !tomove, depth+1, -beta, -alpha);
			}
		}
		
		if (g_stopSearch) {
			//~ for (int i = depth; i < s_maxdepth; i++){
				//~ s_PVtable[depth][i] = s_PVtable[depth + 1][i];
			//~ }
			rmLastRepetition();
			return 0;
		}
		
		if (eval >= beta){
			if (oddity) storePosTT(board.hashValue, -beta, UPPER_BOUND_FLAG, depth, s_maxdepth);
			else storePosTT(board.hashValue, beta, UPPER_BOUND_FLAG, depth, s_maxdepth);
			rmLastRepetition();
			return beta;
		}
		if (alpha < eval){ 
			flag = EXACT_EVAL_FLAG;
			alpha = eval;
			//~ bestindex = i;
			
			if (depth == 0){
				s_nextpos = legalmoves.boards[i].hashValue;
			}
			s_PVtable[depth+1][depth] = boardConvertTomove(&board, &legalmoves.boards[i], tomove);
		}
	}
	rmLastRepetition();
	
	for (int i = depth; i < s_maxdepth; i++){
		s_PVtable[depth][i] = s_PVtable[depth + 1][i];
	}
	
	if (oddity) storePosTT(board.hashValue, -alpha, flag, depth, s_maxdepth);
	else storePosTT(board.hashValue, alpha, flag, depth, s_maxdepth);
	return alpha;
}*/

static inline void emptyPVTable(){
	for (int i = 0; i <= MAXSEARCHDEPTH; i++){
		for (int j = 0; j <= MAXSEARCHDEPTH; j++){
			s_PVtable[i][j] = NULLBITMOVE;
		}
	}
}

move iterativeDeepening(bitboard board, bool tomove){
	s_searchedNodes = 0;
	move nextm = NULLMOVE;
	u64 nextpos = 0;
	
	#ifdef DEBUG
	printBitBoard2d(stdout, board);
	if (info.timeControl) fprintf(debugOutput, "thinking time %d\n", info.moveTime);
	#endif

	g_stopSearch = false;
	info.startTime = getTime_ms();
	int eval = 0, lastEval = 0;
	
	for (int i = 1; i < s_absoluteMaxDepth + 1; i++){
		emptyPVTable();
		s_maxdepth = i;
		s_searchBoard = board;
		eval = search(tomove, i, 0, NegINF, PosINF);
				
		if (s_PVtable[0][0].from != s_PVtable[0][0].to) nextm = convertBitMoveToMove(s_PVtable[0][0]);
		
		bitUndo u = makeMove(&s_searchBoard, s_nextmove);
		nextpos = s_searchBoard.hashValue;
		undoMove(&s_searchBoard, s_nextmove, u);
		
		storePosTT(nextpos, eval, LAST_BEST_EVAL_FLAG, 0, s_maxdepth, &s_nextmove);
		
		if (g_stopSearch){
			printf("info depth %d score cp %d\n", i, lastEval);
			#ifdef DEBUG
			fprintf(debugOutput, "info depth %d score cp %d\n", i, lastEval);
			#endif
			break;
		}
		
		eval *= (tomove == black ? -1 : 1);
		lastEval = eval;
		
		printf("info depth %d", i);
		#ifdef DEBUG
		fprintf(debugOutput, "info depth %d", i);
		#endif
		
		if (eval >= WHITEWON || eval <= BLACKWON){
			if ((tomove == black && eval <= BLACKWON) || (tomove == white && eval >= WHITEWON)){
				//engine is about to win
				printf(" score mate %d pv ", (i_abs(i_abs(eval) - WHITEWON - 100) + 1) / 2);
				#ifdef DEBUG
				fprintf(debugOutput, " score mate %d pv ", (i_abs(i_abs(eval) - WHITEWON - 100) + 1) / 2);
				#endif
			}
			else{
				//we are about to win
				printf(" score mate %d pv ", (i_abs(eval) - WHITEWON - 100 + 1) / 2);
				#ifdef DEBUG
				fprintf(debugOutput, " score mate %d pv ", (i_abs(i_abs(eval) - WHITEWON - 100) + 1) / 2);
				#endif
			}
		}
		else{
			printf(" score cp %d pv ", eval);
			#ifdef DEBUG
			fprintf(debugOutput, " score cp %d pv ", eval);
			#endif
		}
		
		for (int j = 0; j < i && s_PVtable[0][j].from != s_PVtable[0][j].to && !g_stopSearch; j++){
			printmove(stdout, convertBitMoveToMove(s_PVtable[0][j]));
			#ifdef DEBUG
			printmove(debugOutput, convertBitMoveToMove(s_PVtable[0][j]));
			#endif
		}
		printf("\n");
		#ifdef DEBUG
		fprintf(debugOutput, "\n");
		#endif
		
		if (eval >= WHITEWON || eval <= BLACKWON) break; //dont think if not neccesary
	}
	
	//~ rmBestMoveFlag(s_nextpos);
	
	//~ #ifdef DEBUG
	//~ printCollisionStats();
	//~ printf("\n\n");
	//~ #endif 
	
	//in case the search returns a nullmove
	if (nextm.from.file == NULLMOVE.from.file) {
		movearray legalmoves;
		bitGenerateLegalmoves(&legalmoves, &board, tomove, false);
		
		return convertBitMoveToMove(legalmoves.array[0]);
	} 
	return nextm;
}

/* 
 * CPU
 * recieves a move request, starts the thinking with adequate strength
 * */
move CPU(int cpulvl, bitboard bboard, bool tomove){
	squarenums start = {-1, -1};
	move m = initializemove(start, start, 0);
	//~ bitboard bboard = boardConvert(board, castling, enpass, tomove);
	
	if (cpulvl < 0) cpulvl = 0;
	if (cpulvl > 39) cpulvl = 39;
	
	cpulvl = 0;
	
	if (cpulvl == 0){
		#define millisec 1000
		usleep(25 * millisec); 
		m = randomBot(bboard, tomove);
	}	
	else{
		s_absoluteMaxDepth = cpulvl;
		m = iterativeDeepening(bboard, tomove);	
	}
	return m;
}
