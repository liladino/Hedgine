#include "headers/hash.h"

/*
 * The transposition table logic is partly based on Sebastian Lague's 
 * implementation.
 * */

u64 TTableSizeMB = 0;
u64 TTableSize = 0;
TThashentry* TranspositionTable = NULL;

typedef struct key{
	u64 squares[64][12]; //12 pieces
	u64 castlerights[4][2]; /*4 corners indicating the 4 castling directions, and 2 numbers for the right or lack of it.
	* [3] = q
	* [2] = k
	* [1] = Q
	* [0] = K
	* [][0] = cant castle
	* [][1] = can castle
	*/
	u64 enpassantfile[8]; //a = [0], h = [7]
	u64 tomove; //take in, if black to move
}key;

key Zobrist;

TThashentry* allocTransTable(const unsigned int sizeInMB){	
	TTableSizeMB = sizeInMB;
	TTableSize = TTableSizeMB * 1024llu * 1024llu / sizeof(TThashentry);	
	if (sizeInMB == 0){
		TTableSize++;
	}
	TranspositionTable = malloc( TTableSize * sizeof(TThashentry) );
	
	if (TranspositionTable == NULL) return NULL;
	
	clearTransTable();
	
	#ifdef DEBUG
	printDebug( "Transposition Table:\t%lld MB, %lld entry\n", TTableSizeMB, TTableSize);
	printDebug( "Repetition Table:\t%lf kB, %d entry\n", (double) REPETITION_TABLE_SIZE * sizeof(u64) / 1024, REPETITION_TABLE_SIZE);
	#endif
	return TranspositionTable;
}

void freeTransTable(){
	if (TranspositionTable != NULL){
		free(TranspositionTable);
	}
	TranspositionTable = NULL;
	
	TTableSize = TTableSizeMB = 0;
}

void hashPieceIO(bitboard *board, int sq, int piece){
	board->hashValue ^= Zobrist.squares[sq][piece];
}

void hashTomove(bitboard *board){
	board->hashValue ^= Zobrist.tomove;
}

void hashEnPassantIO(bitboard *board, int file){
	if (board->enpassanttarget == 0){
		if (file == 8) return;
		board->hashValue ^= Zobrist.enpassantfile[file];
		return;
	} 
	u64 mask = 0x0101010101010101LL;
	for (int i = 0; i < 8; i++){
		if (mask & board->enpassanttarget){
			board->hashValue ^= Zobrist.enpassantfile[i];
			if (file != 8) { board->hashValue ^= Zobrist.enpassantfile[file]; }
			return;
		}
		mask = mask << 1;
	}
}

void hashCastleO(bitboard *board, uint8_t mask){
	if (board->castlerights & mask){
		int dir = __builtin_ctz(mask);
		//hash the current castleright out, and hash the illegal castling mask in.
		board->hashValue ^= Zobrist.castlerights[dir][1];
		board->hashValue ^= Zobrist.castlerights[dir][0];
	}
}

void setHashKey(){
	for (int i = 0; i < 64; i++){
		for (int j = 0; j < 12; j++){
			Zobrist.squares[i][j] = rand64();
		}
	}
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 2; j++){
			Zobrist.castlerights[i][j] = rand64();
		}
	}
	for (int i = 0; i < 8; i++){
		Zobrist.enpassantfile[i] = rand64();
	}
	Zobrist.tomove = rand64();
}

u64 hashPosition(const bitboard* const board, bool tomove){
	u64 result = 0;

    for (int curr = wking; curr <= bpawn; curr++) {
        u64 bb = board->piece[curr];
        while (bb) {
            int square = __builtin_ctzll(bb);
            result ^= Zobrist.squares[square][curr];
            bb &= bb - 1;  // Clear the LSB
        }
    }

    if (board->enpassanttarget) {
        int file = __builtin_ctzll(board->enpassanttarget) & 7;
        result ^= Zobrist.enpassantfile[file];
    }

    result ^= Zobrist.castlerights[0][(board->castlerights & 1) != 0];
    result ^= Zobrist.castlerights[1][(board->castlerights & 2) != 0];
    result ^= Zobrist.castlerights[2][(board->castlerights & 4) != 0];
    result ^= Zobrist.castlerights[3][(board->castlerights & 8) != 0];

    if (tomove == black) {
        result ^= Zobrist.tomove;
    }

    return result;
}

/*
 * Assumes TT is filled.
 */
TThashentry* lookup(const u64 position){
	const u64 tmp = position % TTableSize;
	if (TranspositionTable[tmp].pos == position){
		return &TranspositionTable[tmp];
	}
	return NULL;
}

void printTransTable(){
	for (size_t i = 0; i < TTableSize; i++){
		if (TranspositionTable[i].pos != 0){
			printf("place: %lld\n", TranspositionTable[i].pos % (TTableSize));
		/*	printf("hash: ");
			printBitPiece(TranspositionTable[i].pos);
			printf("eval: %d\n", TranspositionTable[i].eval);
			printf("nextboard: \n");
			printBitPiece(TranspositionTable[i].next);*/
		}
	}
}

void clearTransTable(){
	for (size_t i = 0; i < TTableSize; i++){
		TranspositionTable[i].pos = 0;
	}
}

#ifdef DEBUG
u64 s_Rcollision = 0;
u64 s_Reads = 0;
u64 s_Wcollision = 0;
u64 s_Writes = 0;

void printCollisionStats(){
	//~ printTransTable();
	printf("TTable reading:\n");
	printf("Collisions: %llu\n", s_Rcollision);
	printf("Collision ratio: %lf\n\n", (double)s_Rcollision / (s_Reads + 1));
	printf("Writing:\n");
	printf("Collisions: %llu\n", s_Wcollision);
	printf("Collision ratio: %lf\n\n", (double)s_Wcollision / (s_Writes + 1));
	
	u64 count = 0;
	for (size_t i = 0; i < TTableSize; i++){
		if (TranspositionTable[i].pos != 0){
			count++;
		}
	}
	printf("\nTTable: \n");
	printf("Filled: %llu\n", count);
	printf("All: %lld\n", TTableSize);
	printf("Ratio to all: %lf\n", (double)count / (TTableSize+1));
	
	//~ s_Rcollision = s_Reads = 0;
	//~ s_Wcollision = s_Writes = 0;
}

#endif

static inline int i_abs(int a){
	return (a < 0 ? -a : a);
}

int correctMateScore(int score, int numPlySearched) {
	if (isMateScore(score)) {
		return (i_abs(score) - numPlySearched) * (score < 0 ? -1 : 1);
	}
	return score;
}

int correctMateScoreForStorage(int score, int numPlySearched) {
	if (isMateScore(score)) {
		return (i_abs(score) + numPlySearched) * (score < 0 ? -1 : 1);
	}
	return score;
}
		
int readHashEntry(u64 hashValue, int remainingDepth, int depth, int alpha, int beta){
	if (!HASHING_ENABLED) {
		return NO_HASH_ENTRY;
	}
	const TThashentry *entry = &TranspositionTable[hashValue % TTableSize];

	if (entry->pos != hashValue) {
		#ifdef DEBUG
		if (0 != entry->pos) s_Rcollision++;
		#endif
		return NO_HASH_ENTRY;
	}
	#ifdef DEBUG
	s_Reads++;
	#endif
	if (entry->depth < remainingDepth){
		return NO_HASH_ENTRY;	
	}
	
	int correctedScore = correctMateScore(entry->eval, depth);
	
	// exact value known
	if (entry->flag == EXACT_EVAL_FLAG) {
		return correctedScore;
	}
	
	// Upper bound known
	if (entry->flag == UPPER_BOUND_FLAG && correctedScore <= alpha) {
		return correctedScore;
	}
	
	// We have stored the lower bound of the eval for this position. Only return if it causes a beta cut-off.
	if (entry->flag == LOWER_BOUND_FLAG && correctedScore >= beta) {
		return correctedScore;
	}

	return NO_HASH_ENTRY;
}

void storePosTT(const u64 hashValue, int eval, evalflag flag, int depthRemaining, int depthSearched, const bitMove* const m) {
	if (!HASHING_ENABLED) {
		return;
	}
	u64 current = hashValue % TTableSize;

	// TThashentry *entry = &TranspositionTable[current];

	// if (entry->depth >= 0 &&
	// 	entry->pos == hashValue &&
	// 	entry->depth > depthRemaining)
	// {
	// 	return;
	// }
	
	#ifdef DEBUG
	if (TranspositionTable[current].pos != 0) s_Wcollision++;
	s_Writes++;
	#endif
	
	TranspositionTable[current].pos = hashValue; //key 
	TranspositionTable[current].eval = correctMateScoreForStorage(eval, depthSearched);
	TranspositionTable[current].flag = flag;
	TranspositionTable[current].depth = depthRemaining;
	TranspositionTable[current].m = *m;
}

void printHashEntry(u64 pos){
	TThashentry* current = lookup(pos);
	if (current == NULL) {
		printf("No record/overweitten\n");
		return;
	}
	printf("%lf %s depth: %d ", current->eval * 0.01/* * (tomove == white ? 1: -1)*/, current->flag == EXACT_EVAL_FLAG ? " EXACT_EVAL_FLAG" : " otherFlag", current->depth);
	printBitPiece(pos);
}

//~ void rmBestMoveFlag(u64 pos){
	//~ TThashentry* current = lookup(pos);
	//~ if (current == NULL) {
		//~ return;
	//~ }
	//~ current->flag = EXACT_EVAL_FLAG;
//~ }

//~ static inline void swap(bitboard* a, bitboard* b) { 
	//~ bitboard temp = *a;
	//~ *a = *b; 
	//~ *b = temp;
//~ } 


int getEval(u64 pos){
	TThashentry *current = &TranspositionTable[pos % TTableSize];
	if (current->pos == pos){
		if (current->flag == EXACT_EVAL_FLAG || current->flag == LOWER_BOUND_FLAG) return current->eval;
	}
	return -1000000;
}

bitMove probeTTMove(u64 pos){
	if (!HASHING_ENABLED || TranspositionTable == NULL || TTableSize == 0)
    	return NULLBITMOVE;  
    TThashentry* entry = &TranspositionTable[pos % TTableSize];
    if (entry->pos == pos) return entry->m;
    return NULLBITMOVE;
}

//~ void orderMoves(movearray* legalmoves){
	//~ for (int i = 1; i < legalmoves->size; i++){
		//~ for (int j = 0; j < legalmoves->size - i; j++){
			//~ if (getEval(legalmoves->array[j].hashValue) < getEval(legalmoves->boards[j+1].hashValue)) swap(&legalmoves->boards[j], &legalmoves->boards[j+1]);
		//~ }
	//~ }
//~ }


/* 
 * REPETITION TABLE
 * */

u64 s_RepetitionTable[REPETITION_TABLE_SIZE];
int g_RTwriteIndex = 0;

/* The last capture, castling or pawn move marks the last position that could be
 * a repetition. When reading the moves from the input, we can reset the write 
 * index to 0, if the move was special. If a new move was searched, we increase
 * the write index, and if the move was taken back, we decrease the write index.
 * This way the new positions constantly get overwritten, while the older ones
 * are kept. If the search is cancelled, we will get new line of moves, so
 * it dowsn't matter if we lost track of the write index. 
 * 
 * The position is considered a DRAW after the FIRST repetition, so the engine
 * avoids previous positions like fire, if it think's it's better. */

// TODO: don't search all the way back to 0 index, only up to the last capture 
bool isRepetition(const u64 pos){
	for (int i = (g_RTwriteIndex > REPETITION_TABLE_SIZE ? REPETITION_TABLE_SIZE : g_RTwriteIndex); i >= 0; i--){
		if (s_RepetitionTable[i] == pos)	return true;
	}
	
	return false;
}

void storeRepetiton(const u64 pos){
	g_RTwriteIndex++;
	
	if (g_RTwriteIndex < REPETITION_TABLE_SIZE){
		//store the position if there's room for it	
		s_RepetitionTable[g_RTwriteIndex] = pos;
	}
} 

void rmLastRepetition(){
	if (g_RTwriteIndex > 0) g_RTwriteIndex--;
}
