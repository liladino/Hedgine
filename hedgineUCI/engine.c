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
#define MAX_QSEARCH_DEPTH 16
#define MAXPLY (MAXSEARCHDEPTH + MAX_QSEARCH_DEPTH) 
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
static bool     s_followPV;
//heuristics
static int      s_history[12][64]; //beta cutoff storing
static bitMove  s_killers[2][MAXSEARCHDEPTH+1];
#ifdef DEBUG
	   u64		s_cutoffs = 0;
	   u64		s_cutoffsFirst = 0;
#endif
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
	return i_abs(score) > MATE_NOW - MAXPLY;
}

//nodes searched by the last iterativeDeepening call (search + quiescence)
u64 getSearchedNodes() {
	return s_searchedNodes;
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
move randomBot(bitboard board, sidetomove tomove){
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
static int quiescenceSearch(sidetomove tomove, int alpha, int beta, int ply, int qdepth){
	s_searchedNodes++;
	if (1026 == s_searchedNodes % 1027) { communicate(); }
	if (g_stopSearch) { return 0; }

	if (s_searchBoard.fmv >= 100 &&
		!(bitInCheck(&s_searchBoard, tomove) 
		&& !hasLegalMove(&s_searchBoard, tomove))
	){
		return 0;
	}

	if (qdepth >= MAX_QSEARCH_DEPTH) {
    	return staticEval(&s_searchBoard, tomove);
	}

	if (isRepetition(s_searchBoard.hashValue, s_searchBoard.fmv)){
		return 0;
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

	storeRepetiton(s_searchBoard.hashValue);

	int actualLegalMoves = 0;

	MovePicker picker;
	initMovePicker(
		&picker,
		&NULLBITMOVE,
		&NULLBITMOVE,
		&NULLBITMOVE,
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
				rmLastRepetition();
				return 0; 
			}
			continue;
		}

		++actualLegalMoves;

		int eval = -quiescenceSearch(!tomove, -beta, -alpha, ply+1, qdepth+1);
		undoMove(&s_searchBoard, &currentMove, &u);
		if (g_stopSearch) { 
			rmLastRepetition();
			return 0; 
		}

		if (eval >= beta) {
			rmLastRepetition();
			return beta;
		}
		alpha = i_max(alpha, eval);
	}
	rmLastRepetition();
	
	if (0 == actualLegalMoves) {
		if (inCheck){
			return -(MATE_NOW - ply);
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
int search(sidetomove tomove, int remainingDepth, int depth, int alpha, int beta, int numExtensions){	
	if (depth >= MAXSEARCHDEPTH) {
		return staticEval(&s_searchBoard, tomove);
	}

	s_PVlen[depth] = depth;
	
	s_searchedNodes++;
	if (2026 == s_searchedNodes % 2027) { communicate(); }
	if (g_stopSearch) { return 0; }
	
	if (0 < depth) {
		if (s_searchBoard.fmv >= 100 &&
			!(bitInCheck(&s_searchBoard, tomove) 
			&& !hasLegalMove(&s_searchBoard, tomove))
		){
			return 0;
		}
		if (isRepetition(s_searchBoard.hashValue, s_searchBoard.fmv)) { return 0; }
		
		alpha = i_max(alpha, -MATE_NOW + depth);
		beta  = i_min(beta,   MATE_NOW - depth - 1);
		if (alpha >= beta) {
			return alpha;
		}
	}
	
	TThashentry *probed = NULL;
	int eval = readHashEntry(s_searchBoard.hashValue, remainingDepth, depth, alpha, beta, &probed);
	//~ int eval = NO_HASH_ENTRY;
	if (NO_HASH_ENTRY != eval && depth > 0){
		s_PVlen[depth] = depth; 
		s_PVtable[depth][depth] = NULLBITMOVE;
		return eval;
	}
	
	if (0 == remainingDepth) {
		return quiescenceSearch(tomove, alpha, beta, depth, 0);
	}

	/*
	 * Best move hint
	 * */
	bitMove hint = NULLBITMOVE;
	if (s_followPV && depth < s_PVcommitlen) {
		hint = s_PVcommit[depth];
	} 
	else {
		s_followPV = false;
		if (depth > 0 && probed) {
			hint = probed->m;
		}
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

		//do we follow PV?
		if (s_followPV && !sameMove(&currentMove, &hint)){
			s_followPV = false;
		}

		/*
		 * Extension?
		 * */
		int extension = 0;
		if (numExtensions < MAX_EXTENSIONS) {
			if (inCheck) {
				extension = 1;
			}
		}

		bool isQuiet = !(currentMove.flags & (CAPTURE_FLAG | EN_PASSANT_FLAG | PROMOTION_FLAG));

		/*
		 * Reduce?
		 * */
		bool needsFullSearch = true;
		if (!inCheck && extension == 0 && remainingDepth >= 3 && actualLegalMoves >= 3 && isQuiet) {
			// if (!bitInCheck(&s_searchBoard, !tomove)){ }
			eval = -search(!tomove, remainingDepth-1-1, depth+1, -alpha-1, -alpha, numExtensions);
			needsFullSearch = eval > alpha;
		}
		
		/*
		 * Search
		 * */
		if (needsFullSearch){
			eval = -search(!tomove, remainingDepth-1+extension, depth+1, -beta, -alpha, numExtensions + extension);
		}
		undoMove(&s_searchBoard, &currentMove, &u);

		if (g_stopSearch) {
			rmLastRepetition();
			return 0;
		}
		
		/*
		 * Pruning
		 * */
		if (eval >= beta) {
			#ifdef DEBUG
			s_cutoffs++;
			if (actualLegalMoves == 1){
				s_cutoffsFirst++;
			}
			#endif

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
		return inCheck ? -(MATE_NOW - depth) : 0;
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

move iterativeDeepening(bitboard board, sidetomove tomove){
	long int searchStartTime = getTime_ms();
	
	s_searchedNodes = 0;
	move nextm = NULLMOVE;
	emptyPVTable();
	emptyKillers();
	newSearchTT();
	
	s_PVcommit[0] = NULLBITMOVE;
	s_PVcommitlen = 0;
	
	#ifdef DEBUG
	// printBitBoard2d(stdout, board);
	if (info.timeControl) printf("thinking time %d\n", info.moveTime);
	#endif

	g_stopSearch = false;
	int eval = 0, lastEval = 0;
	
	for (int i = 1; i < s_searchDepthLimit + 1; i++){
		#ifdef DEBUG
		s_cutoffs = s_cutoffsFirst = 0;
		#endif
		ageHistory();
		s_maxdepth = i;
		s_searchBoard = board;
		s_followPV = true;
		eval = search(tomove, i, 0, NegINF, PosINF, 0);
		if (!sameMove(&NULLBITMOVE, &s_PVcommit[0])) nextm = convertBitMoveToMove(s_PVcommit[0]);
		
		// usleep(500);
		communicate();
					
		if (g_stopSearch){
			//iteration i was cut short, so the last reliable result is from depth i-1
			if (i > 1){
				printf("info depth %d score cp %d\n", i-1, lastEval);
			}
			#ifdef DEBUG
			printf("cutoffs:\n%llu \tin the first move\n%llu \toverall\n\n", s_cutoffsFirst, s_cutoffs);
			#endif
			break;
		}

		lastEval = eval;

		//the whole line is built first, so it is printed with one printf (one timestamp in the debug log)
		char line[64 + (MAXSEARCHDEPTH+1) * MOVESTR_LEN];
		int len = snprintf(line, sizeof(line), "info depth %d", i);

		bool mate = false;
		if (i_abs(eval) >= WHITEWON-1000){
			//eval is from the engine's perspective: positive => the engine mates
			int mateIn = (WHITEWON - i_abs(eval) + 1) / 2;
			len += snprintf(line + len, sizeof(line) - len, " score mate %d pv ", eval > 0 ? mateIn : -mateIn);
			if (mateIn * 2 <= i) mate = true;
		}
		else{
			len += snprintf(line + len, sizeof(line) - len, " score cp %d pv ", eval);
		}

		for (int j = 0; j < s_PVcommitlen && j < i && !g_stopSearch; j++){
			len += moveToString(line + len, convertBitMoveToMove(s_PVcommit[j]));
		}
		printf("%s\n", line);

		#ifdef DEBUG
		printf("cutoffs:\n%llu \tin the first move\n%llu \toverall\n\n", s_cutoffsFirst, s_cutoffs);
		#endif

		if (mate) break;
	}
	
	// #ifdef DEBUG
	// printCollisionStats();
	// printf("\n\n");
	// #endif 
	
	// if we return the move too quickly, some GUIs don't register it
	// (not under time control: on Windows the sleep can last a whole ~15ms scheduler tick)
	if (!info.timeControl && !g_stopSearch && getTime_ms() - searchStartTime < 1000) {
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
move CPU(int cpulvl, bitboard bboard, sidetomove tomove){
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
