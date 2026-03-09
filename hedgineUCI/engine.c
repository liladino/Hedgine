#include "headers/engine.h"

/*
 * CONSTANTS 
 * */
const move    NULLMOVE = {{-1, -1}, {-1, -1}, 0};
const bitMove NULLBITMOVE = {0, 0, 0, -1, 0};
#define PosINF   2147480000  //less than 2^31 to avoid possible overflow
#define NegINF  -2147480000
#define MATE_NOW WHITEWON //2147400000 much less than infinities
#define MAXSEARCHDEPTH 40 

/*
 * STATICS & GLOBALS
 * */
//current max depth and limit (engine strength)
static int      s_maxdepth;
static int      s_searchDepthLimit = MAXSEARCHDEPTH;
//search
static u64      s_searchedNodes;
static bitboard s_searchBoard; //the search takes place on this
//PV
static bitMove  s_PVtable [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];
static int      s_PVlen [MAXSEARCHDEPTH]; // length of PV line at each depth
static bitMove  s_PVcommit [MAXSEARCHDEPTH+1];
static int      s_PVcommitlen;
//heuristics
static int      s_history[12][64]; //beta cutoff storing
static bitMove  s_killers[2][MAXSEARCHDEPTH+1];
//golbals
       bool     g_stopSearch = false;

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
	//~ printf("ring-a-ding baby~!\n");
	if(info.timeControl == true && getTime_ms()-info.startTime > info.moveTime) {
		g_stopSearch = true;
	}
	
	readInput();
}

/* 
 * RANDOM MOVER BOT
 *  */
move randomBot(bitboard board, bool tomove){
	static bool called = false;
	if (!called){
		srand((unsigned) time (NULL));
		called = true;
	}
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, &board, tomove, false);
	
	int legals[MAXMOVECOUNT_INPOS] = {0};
	int j = 0;
	for (int i = 0; i < legalmoves.size; i++){
		if (!isCastlingLegal(&board, tomove, &legalmoves.array[i])){
			continue;
		}

		bitUndo u = makeMove(&board, legalmoves.array[i]);
		bool b = bitInCheck(&board, tomove);
		undoMove(&board, legalmoves.array[i], u);
		
		if (b){ continue; }
		
		legals[j++] = i;
	}
	
	int i = legals[rand() % j];
	return convertBitMoveToMove(legalmoves.array[i]);
}

/*
 * Order moves
 * */
typedef struct pair {
	int eval, index;
} pair;

static void insertionSort(pair a[], int n) {
	for (int i = 1; i < n; i++) {
		pair keyy = a[i];
		int j = i - 1;
		while (j >= 0 && a[j].eval < keyy.eval) { // DESC
			a[j + 1] = a[j];
			j--;
		}
		a[j + 1] = keyy;
	}
}

static inline bool sameMove(const bitMove* const a, const bitMove* const b) {
	return a->from == b->from &&
		   a->to == b->to &&
		   a->piece == b->piece &&
		   a->promotion == b->promotion &&
		   a->flags == b->flags;
}

static int scoreMove(bitMove* m, bitMove* lastBest, int depth) {
	static int pieceValue[12] = { 0, 900, 500, 300, 300, 100, 0, 900, 500, 300, 300, 100 };
	if (sameMove(m, lastBest)) { return 1000000; }
	if (m->flags & PROMOTION_FLAG) { return 900000 + pieceValue[m->promotion]; }
	if (m->flags & (CAPTURE_FLAG | EN_PASSANT_FLAG)) { return 800000 + attackValue(m->from, m->to); }
	
	if (sameMove(m, &s_killers[0][depth])) return 700000;
	if (sameMove(m, &s_killers[1][depth])) return 650000;
	
	return s_history[m->piece][m->to];
}

static void orderMoves(movearray* legalmoves, bitMove* lastBest, int depth) {
	pair moveScores[MAXMOVECOUNT_INPOS];
	for (int i = 0; i < legalmoves->size; i++) {
		moveScores[i] = (pair){scoreMove(&legalmoves->array[i], lastBest, depth), i};
	}
	insertionSort(moveScores, legalmoves->size);
	
	movearray copy = *legalmoves;
	for (int i = 0; i < legalmoves->size; i++) {
		legalmoves->array[i] = copy.array[moveScores[i].index];
	}
}

/*
 * Quiescence search
 * */
static int quiescenceSearch(bool tomove, int alpha, int beta, int depth){
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }

	bool inCheck = bitInCheck(&s_searchBoard, tomove);

	if (!inCheck) {
		//~ printBitBoard2d(stdout, s_searchBoard);
		int stand = fulleval(&s_searchBoard, tomove, s_maxdepth);
		if (stand >= beta) return beta;
		if (stand > alpha) alpha = stand;
	}

	movearray moves;
	if (inCheck) {
		// could benefit from generating e.g. evasions only
		bitGenerateLegalmoves(&moves, &s_searchBoard, tomove, false); 
	} else {
		bitGenerateLegalmoves(&moves, &s_searchBoard, tomove, true);  
	}

	int legalFound = 0;

	for (int i = 0; i < moves.size; i++) {
		bitUndo u = makeMove(&s_searchBoard, moves.array[i]);
		if (bitInCheck(&s_searchBoard, tomove)) {
			undoMove(&s_searchBoard, moves.array[i], u);
			continue;
		}
		legalFound++;

		int eval = -quiescenceSearch(!tomove, -beta, -alpha, depth+1);
		undoMove(&s_searchBoard, moves.array[i], u);

		if (eval >= beta) return beta;
		alpha = i_max(alpha, eval);
	}
	
	// mate
	if (inCheck && 0 == legalFound) {
		return -(MATE_NOW - depth);
	}

	return alpha;
}

/* NegaMax search
 * 
 * depth: from the root node, 0: nothing searched yet
 * remainingDepth: decreasing to 0
 * 
 * Credits partially to Sebastian Lague
 * */
int search(bool tomove, int remainingDepth, int depth, int alpha, int beta){	
	s_PVlen[depth] = depth;
	
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }
	
	if (0 < depth) {
		if (isRepetition(s_searchBoard.hashValue)) return -1;
		
		alpha = i_max(alpha, -MATE_NOW + depth);
		beta  = i_min(beta,   MATE_NOW - depth);
		if (alpha >= beta) {
			return alpha;
		}
	}
	
	int eval = readHashEntry(s_searchBoard.hashValue, remainingDepth, depth, alpha, beta);
	//~ int eval = NO_HASH_ENTRY;
	if (NO_HASH_ENTRY != eval && depth > 0){
		s_PVlen[depth] = depth; 
		s_PVtable[depth][depth] = NULLBITMOVE;
		return eval;
	}
	
	if (0 == remainingDepth) {
		return quiescenceSearch(tomove, alpha, beta, depth);
	}

	/*
	 * Generate pseudo legal moves
	 * 
	 * Check later, whether any of them was actually legal.
	 * */
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, &s_searchBoard, tomove, false);
	int actualLegalMoves = 0;
	
	/*
	 * Order moves
	 * */
	if (depth == 0){
		bitMove hint = (s_PVcommitlen > 0) ? s_PVcommit[0] : NULLBITMOVE;
		orderMoves(&legalmoves, &hint, depth);
	}
	else {
		bitMove ttMove = probeTTMove(s_searchBoard.hashValue);
		orderMoves(&legalmoves, &ttMove, depth);
	}
	
	/*
	 * Store repetition
	 * */
	storeRepetiton(s_searchBoard.hashValue);
	
	evalflag moveFlag = UPPER_BOUND_FLAG;
	bitMove* bestMove = &legalmoves.array[0];

	for (int i = 0; i < legalmoves.size; i++) { 
		#ifdef DEBUG
		if (depth == 0){
			i++; i--;
		}
		#endif
		
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
		
		actualLegalMoves++;
		
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
		eval = -search(!tomove, remainingDepth-1, depth+1, -beta, -alpha);
		undoMove(&s_searchBoard, *currentMove, u);
		
		if (g_stopSearch) {
			rmLastRepetition();
			return 0;
		}
		
		/*
		 * Pruning
		 * */
		if (eval >= beta) {
			//the "refutation"
			storePosTT(s_searchBoard.hashValue, beta, LOWER_BOUND_FLAG, remainingDepth, depth, currentMove); //probably bestMove instead of currentMove?? - not sure, becase the currentMove wa 
			
			/*
			 * Killers and history
			 * */
			if (!(currentMove->flags & CAPTURE_FLAG) && !(currentMove->flags & PROMOTION_FLAG)) {
				if (sameMove(currentMove, &s_killers[1][depth])) {
					// promote 
					s_killers[1][depth] = s_killers[0][depth];
					s_killers[0][depth] = *currentMove;
				} else if (!sameMove(currentMove, &s_killers[0][depth])) {
					// new killer
					s_killers[1][depth] = s_killers[0][depth];
					s_killers[0][depth] = *currentMove;
				}
				
				s_history[currentMove->piece][currentMove->to] += depth * depth;
			}
			
			rmLastRepetition();

			return beta;
		}
		 
		/*
		 * New best move foud
		 * */
		if (eval > alpha) {
			moveFlag = EXACT_EVAL_FLAG;
			bestMove = currentMove; //for TT
			
			alpha = eval;
			
			s_PVtable[depth][depth] = *currentMove;
			int currentLen = (s_PVlen[depth+1] > depth+1) ? s_PVlen[depth+1] : (depth+1);
			s_PVlen[depth] = currentLen;

			for (int j = depth+1; j < currentLen; j++){
				s_PVtable[depth][j] = s_PVtable[depth+1][j];
			}
			s_PVtable[depth][currentLen] = NULLBITMOVE; 
		}
	}
	
	rmLastRepetition();
		
	if (0 == actualLegalMoves){
		if (bitInCheck(&s_searchBoard, tomove)) {
			return -(MATE_NOW - depth);
		}
		else {
			return 0;
		}
	}
		
		/*
position fen rnbqkbnr/pppp1ppp/4p3/8/5PP1/8/PPPPP2P/RNBQKBNR b KQkq - 0 2
go infinite

position fen rnbqkbnr/pppp1ppp/4p3/8/5PP1/8/PPPPP2P/RNBQKBNR b KQkq - 0 2
go movetime 1
go movetime 1111
		 */
	
	storePosTT(s_searchBoard.hashValue, alpha, moveFlag, remainingDepth, depth, bestMove);	
	
	if (depth == 0 && !g_stopSearch) {
		s_PVcommitlen = s_PVlen[0];
		for (int k = 0; k < s_PVcommitlen; k++) {
			s_PVcommit[k] = s_PVtable[0][k];
		}
	}
	
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

static inline void emptyKillers(){
	for (int i = 0; i <= 1; i++){
		for (int j = 0; j <= MAXSEARCHDEPTH; j++){
			s_killers[i][j] = NULLBITMOVE;
		}
	}
}

static inline void ageHistory(){
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 64; j++){
			s_history[i][j] >>= 1;
		}
	}
}

move iterativeDeepening(bitboard board, bool tomove){
	long int searchStartTime = getTime_ms();
	
	s_searchedNodes = 0;
	move nextm = NULLMOVE;
	emptyPVTable();
	emptyKillers();
	
	#ifdef DEBUG
	printBitBoard2d(stdout, board);
	if (info.timeControl) fprintf(g_debugOutput, "thinking time %d\n", info.moveTime);
	#endif

	g_stopSearch = false;
	info.startTime = getTime_ms();
	int eval = 0, lastEval = 0;
	
	for (int i = 1; i < s_searchDepthLimit + 1; i++){
		ageHistory();
		s_maxdepth = i;
		s_searchBoard = board;
		eval = search(tomove, i, 0, NegINF, PosINF);
				
		if (!sameMove(&NULLBITMOVE, &s_PVcommit[0])) nextm = convertBitMoveToMove(s_PVcommit[0]);
		
		//~ usleep(500);
		communicate();
		
		//~ { /* store the best move */
			//~ bitUndo u = makeMove(&s_searchBoard, s_PVcommit[0]);
			//~ nextpos = s_searchBoard.hashValue;
			//~ undoMove(&s_searchBoard, s_PVcommit[0], u);
		
			//~ storePosTT(nextpos, eval, LAST_BEST_EVAL_FLAG, 0, s_maxdepth, &s_PVcommit[0]);
		//~ }
			
		if (g_stopSearch){
			printf("info depth %d score cp %d\n", i, lastEval);
			#ifdef DEBUG
			fprintf(g_debugOutput, "info depth %d score cp %d\n", i, lastEval);
			#endif
			break;
		}
		
		eval *= (tomove == black ? -1 : 1);
		lastEval = eval;
		
		printf("info depth %d", i);
		#ifdef DEBUG
		fprintf(g_debugOutput, "info depth %d", i);
		#endif
		
		bool mate = false;
		if (i_abs(eval) >= WHITEWON-1000){
			int mateIn = 1e8;
			if ((tomove == black && eval <= BLACKWON+1000) || (tomove == white && eval >= WHITEWON-1000)){
				//engine is about to win
				mateIn = (i_abs(i_abs(eval) - WHITEWON) + 1) / 2;
				printf(" score mate %d pv ", mateIn);
				#ifdef DEBUG
				fprintf(g_debugOutput, " score mate %d pv ", (i_abs(i_abs(eval) - WHITEWON) + 1) / 2);
				#endif
			}
			else{
				//we are about to win
				mateIn = (i_abs(i_abs(eval) - WHITEWON) + 1) / 2;
				printf(" score mate %d pv ", -mateIn);
				#ifdef DEBUG
				fprintf(g_debugOutput, " score mate %d pv ", -(i_abs(i_abs(eval) - WHITEWON) + 1) / 2);
				#endif
			}
			if (mateIn * 2 <= i) mate = true;
		}
		else{
			printf(" score cp %d pv ", eval);
			#ifdef DEBUG
			fprintf(g_debugOutput, " score cp %d pv ", eval);
			#endif
		}
		
		for (int j = 0; j < s_PVcommitlen && j < i && !g_stopSearch; j++){
			printmove(stdout, convertBitMoveToMove(s_PVcommit[j]));
			#ifdef DEBUG
			printmove(g_debugOutput, convertBitMoveToMove(s_PVtable[0][j]));
			#endif
		}
		printf("\n");
		#ifdef DEBUG
		fprintf(g_debugOutput, "\n");
		#endif
		
		if (mate) break;
	}
	
	//~ rmBestMoveFlag(s_nextpos);
	
	//~ #ifdef DEBUG
	//~ printCollisionStats();
	//~ printf("\n\n");
	//~ #endif 
	
	// if we return the move too quickly, some GUIs don't register it
	if (!g_stopSearch && getTime_ms() - searchStartTime < 1000) {
		usleep(1000); 
	}
	
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
	
	//~ cpulvl = 0;
	
	if (cpulvl == 0){
		#define millisec 1000
		usleep(25 * millisec); 
		m = randomBot(bboard, tomove);
	}	
	else{
		s_searchDepthLimit = cpulvl;
		//~ s_searchDepthLimit = 2;
		m = iterativeDeepening(bboard, tomove);	
	}
	return m;
}
