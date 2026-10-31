#include "headers/engine.h"

/*
 * CONSTANTS 
 * */
const move    NULLMOVE = {{-1, -1}, {-1, -1}, 0};
const bitMove NULLBITMOVE = {0, 0, 0, -1, 0};
#define MAX_EXTENSIONS 10
#define PosINF   2147480000  //less than 2^31 to avoid possible overflow
#define NegINF  -2147480000
#define MATE_NOW WHITEWON //2147400000 much less than infinities
#define MAXSEARCHDEPTH 64 
#define HISTORY_MAX 16384 //well below the killer move scores in move picker

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
static bitMove  s_PVtable  [MAXSEARCHDEPTH+1][MAXSEARCHDEPTH+1];
static int      s_PVlen    [MAXSEARCHDEPTH+1]; // length of PV line at each depth
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

/* "History gravity": the closer an entry is to HISTORY_MAX, the less a bonus
 * moves it, so the values saturate smoothly in [-HISTORY_MAX, HISTORY_MAX]
 * instead of growing without a bound.
 * */
static inline void updateHistory(int *entry, int bonus){
	bonus = i_max(-HISTORY_MAX, i_min(HISTORY_MAX, bonus));
	*entry += bonus - *entry * i_abs(bonus) / HISTORY_MAX;
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
	generateLegalMoves(&legalmoves, &board, tomove);

	if (legalmoves.size == 0){
		return NULLMOVE;
	}
	return convertBitMoveToMove(legalmoves.array[rand() % legalmoves.size]);
}

/*
 * Quiescence search
 * */
static int quiescenceSearch(bool tomove, int alpha, int beta, int depth){
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }

	if (depth >= MAXSEARCHDEPTH) {
    	return staticEval(&s_searchBoard, tomove);
	}

	bool inCheck = bitInCheck(&s_searchBoard, tomove);

	if (!inCheck) {
		//~ printBitBoard2d(stdout, s_searchBoard);
		int stand = staticEval(&s_searchBoard, tomove);
		if (stand >= beta) {
			return hasLegalMove(&s_searchBoard, tomove) ? beta : 0;
		}
		if (stand > alpha){
			alpha = stand;
		}
	}

	int actualLegalMoves = 0;

	MovePicker picker;
	initMovePicker(
		&picker,
		&NULLBITMOVE,
		&s_killers[0][depth],
		&s_killers[1][depth],
		(const int (*)[64])s_history,
		inCheck,
		true
	);
	bitMove currentMove;
	while (nextMove(&picker, &s_searchBoard, tomove, &currentMove)) {
		if (!isCastlingLegal(&s_searchBoard, tomove, &currentMove)){
			continue;
		}
		bitUndo u = makeMove(&s_searchBoard, &currentMove);
		if (bitInCheck(&s_searchBoard, tomove)){
			undoMove(&s_searchBoard, &currentMove, &u);
			if (g_stopSearch) { 
				return 0; 
			}
			continue;
		}

		++actualLegalMoves;

		int eval = -quiescenceSearch(!tomove, -beta, -alpha, depth+1);
		undoMove(&s_searchBoard, &currentMove, &u);
		if (g_stopSearch) { 
			return 0; 
		}

		if (eval >= beta) {
			return beta;
		}
		alpha = i_max(alpha, eval);
	}
	
	if (0 == actualLegalMoves) {
		if (inCheck){
			return -(MATE_NOW - depth);
		}
		if (!hasLegalMove(&s_searchBoard, tomove)){
        	return 0;
		}
	}

	return alpha;
}

/* NegaMax search
 * 
 * depth: from the root node, 0: nothing searched yet
 * remainingDepth: decreasing to 0
 * */
int search(bool tomove, int remainingDepth, int depth, int alpha, int beta, int numExtensions){	
	if (depth >= MAXSEARCHDEPTH) {
		return staticEval(&s_searchBoard, tomove);
	}

	s_PVlen[depth] = depth;
	
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }
	
	if (0 < depth) {
		if (isRepetition(s_searchBoard.hashValue)) { return 0; }
		
		alpha = i_max(alpha, -MATE_NOW + depth);
		beta  = i_min(beta,   MATE_NOW - depth - 1);
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
	 * Order moves
	 * */
	bitMove hint;
	if (depth == 0){
		hint = (s_PVcommitlen > 0) ? s_PVcommit[0] : NULLBITMOVE;
	}
	else {
		hint = probeTTMove(s_searchBoard.hashValue);
	}
	
	/*
	 * Store repetition
	 * */
	storeRepetiton(s_searchBoard.hashValue);

	/*
	 * Generate pseudo legal moves
	 * 
	 * Check later, whether any of them was actually legal.
	 * */
	bool inCheck = bitInCheck(&s_searchBoard, tomove);
	MovePicker picker;
	initMovePicker(
		&picker,
		&hint,
		&s_killers[0][depth],
		&s_killers[1][depth],
		(const int (*)[64])s_history,
		inCheck,
		false
	);
	int actualLegalMoves = 0;

	evalflag moveFlag = UPPER_BOUND_FLAG;

	bitMove currentMove;
	bitMove bestMove = NULLBITMOVE;
	//quiet moves searched so far, that failed to produce a cutoff (for the history malus)
	bitMove quietsTried[MAXMOVECOUNT_INPOS];
	int quietCount = 0;
	while (nextMove(&picker, &s_searchBoard, tomove, &currentMove)) {
		if (!isCastlingLegal(&s_searchBoard, tomove, &currentMove)){
			continue;
		}
		bitUndo u = makeMove(&s_searchBoard, &currentMove);
		if (bitInCheck(&s_searchBoard, tomove)){
			undoMove(&s_searchBoard, &currentMove, &u);
			continue;
		}

		++actualLegalMoves;

		/*
		 * Extension?
		 * */
		
		int extension = 0;
		if (numExtensions < MAX_EXTENSIONS) {
			if (inCheck) {
				extension = 1;
			}
		}

		/*
		 * Reduce?
		 * (not interesting moves, e.g. late in ordering)
		 * */
		 
		//~ TODO
		
		/*
		 * Search
		 * */
		eval = -search(!tomove, remainingDepth-1+extension, depth+1, -beta, -alpha, numExtensions + extension);
		undoMove(&s_searchBoard, &currentMove, &u);

		if (g_stopSearch) {
			rmLastRepetition();
			return 0;
		}
		
		bool isQuiet = !(currentMove.flags & CAPTURE_FLAG) && !(currentMove.flags & PROMOTION_FLAG);

		/*
		 * Pruning
		 * */
		if (eval >= beta) {
			//the "refutation"
			storePosTT(s_searchBoard.hashValue, beta, LOWER_BOUND_FLAG, remainingDepth, depth, &currentMove); 
			
			/*
			 * Killers and history
			 * */
			if (isQuiet) {
				if (sameMove(&currentMove, &s_killers[1][depth])) {
					// promote 
					s_killers[1][depth] = s_killers[0][depth];
					s_killers[0][depth] = currentMove;
				} else if (!sameMove(&currentMove, &s_killers[0][depth])) {
					// new killer
					s_killers[1][depth] = s_killers[0][depth];
					s_killers[0][depth] = currentMove;
				}
				
				//cutoffs higher up in the tree prune more, so they weigh more
				int bonus = remainingDepth * remainingDepth;
				updateHistory(&s_history[currentMove.piece][currentMove.to], bonus);

				//malus: the quiets searched before this one should have been ordered later
				for (int k = 0; k < quietCount; k++){
					updateHistory(&s_history[quietsTried[k].piece][quietsTried[k].to], -bonus);
				}
			}
			
			rmLastRepetition();

			return beta;
		}

		if (isQuiet){
			quietsTried[quietCount++] = currentMove;
		}

		/*
		 * New best move foud
		 * */
		if (eval > alpha) {
			moveFlag = EXACT_EVAL_FLAG;
			bestMove = currentMove; //for TT
			
			alpha = eval;
			
			s_PVtable[depth][depth] = currentMove;
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
	
	storePosTT(s_searchBoard.hashValue, alpha, moveFlag, remainingDepth, depth, &bestMove);	
	
	if (depth == 0 && !g_stopSearch) {
		s_PVcommitlen = s_PVlen[0];
		for (int k = 0; k < s_PVcommitlen; k++) {
			s_PVcommit[k] = s_PVtable[0][k];
		}
	}
	
	return alpha;
}

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
	
	s_PVcommit[0] = NULLBITMOVE;
	s_PVcommitlen = 0;
	
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
		eval = search(tomove, i, 0, NegINF, PosINF, 0);
		if (!sameMove(&NULLBITMOVE, &s_PVcommit[0])) nextm = convertBitMoveToMove(s_PVcommit[0]);
		
		//~ usleep(500);
		communicate();
					
		if (g_stopSearch){
			//iteration i was cut short, so the last reliable result is from depth i-1
			if (i > 1){
				printf("info depth %d score cp %d\n", i-1, lastEval);
				#ifdef DEBUG
				fprintf(g_debugOutput, "info depth %d score cp %d\n", i-1, lastEval);
				#endif
			}
			break;
		}

		lastEval = eval;

		printf("info depth %d", i);
		#ifdef DEBUG
		fprintf(g_debugOutput, "info depth %d", i);
		#endif

		bool mate = false;
		if (i_abs(eval) >= WHITEWON-1000){
			//eval is from the engine's perspective: positive => the engine mates
			int mateIn = (WHITEWON - i_abs(eval) + 1) / 2;
			printf(" score mate %d pv ", eval > 0 ? mateIn : -mateIn);
			#ifdef DEBUG
			fprintf(g_debugOutput, " score mate %d pv ", eval > 0 ? mateIn : -mateIn);
			#endif
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
	
	//in case the search was cancelled before the first iteration finished
	if (nextm.from.file == NULLMOVE.from.file) {
		movearray legalmoves;
		generateLegalMoves(&legalmoves, &board, tomove);

		if (legalmoves.size == 0){
			return NULLMOVE;
		}
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
