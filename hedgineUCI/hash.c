#include "hash.h"

int TTableSizeMB = 0;
int TTableSize = 0;
TThashentry* TranspositionTable = NULL;

TThashentry* allocTransTable(const int sizeInMB){
	TTableSizeMB = sizeInMB;
	TTableSize = TTableSizeMB * 1024 * 1024 / sizeof(TThashentry);	
	if (sizeInMB == 0){
		TTableSize++;
	}
	TranspositionTable = malloc( TTableSize * sizeof(TThashentry) );
	
	if (TranspositionTable == NULL) return NULL;
	
	clearTransTable();
	
	#ifdef DEBUG
	fprintf(debugOutput, "Transposition Table:\t%d MB, %d entry\n", TTableSizeMB, TTableSize);
	fprintf(debugOutput, "Repetition Table:\t%lf kB, %d entry\n", (double) REPETITION_TABLE_SIZE * sizeof(u64) / 1024, REPETITION_TABLE_SIZE);
	#endif
	return TranspositionTable;
}

void freeTransTable(){
	if (TranspositionTable != NULL){
		free(TranspositionTable);
	}
	
	TTableSize = TTableSizeMB = 0;
}

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

void hashPieceIO(bitboard *board, int sq, int piece){
	board->hashValue ^= Zobrist.squares[sq][piece];
}

void hashTomoveIO(bitboard *board){
	board->hashValue ^= Zobrist.tomove;
}

void hashEnPassantIO(bitboard *board, int file){
	if (board->enpassanttarget == 0){
		if (file == 8) return;
		//board->hashValue ^= Zobrist.enpassantfile[8];
		board->hashValue ^= Zobrist.enpassantfile[file];
		return;
	} 
	u64 mask = 0x0101010101010101LL;
	for (int i = 0; i < 8; i++){
		if (mask & board->enpassanttarget){
			board->hashValue ^= Zobrist.enpassantfile[i];
			if (file != 8) board->hashValue ^= Zobrist.enpassantfile[file];
			return;
		}
		mask = mask << 1;
	}
}

void hashCastleO(bitboard *board, int dir){
	char mask = 1 << dir; //K
	if (board->castlerights & mask){
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
	/*u64 result = 0;
	u64 mask = 1;
	for (int i = 0; i < 64; i++){
		for (int j = 0; j < 12; j++){
			if (board->piece[j] & mask){
				result ^= Zobrist.squares[i][j];
				break;
			}
		}
		mask = mask << 1;
	}
	if (board->enpassanttarget){
		mask = 0x0101010101010101LL; //a file
		for (int i = 0; i < 8; i++){
			if (mask & board->enpassanttarget){
				result ^= Zobrist.enpassantfile[i];
				break;
			}
			mask = mask << 1;
		}
	}
	//else result ^= Zobrist.enpassantfile[8];
	mask = 1; //K
	for (int i = 0; i < 4; i++){
		if (board->castlerights & mask) result ^= Zobrist.castlerights[i][1];
		else result ^= Zobrist.castlerights[i][0];
		
		mask = mask << 1;
	}
	if (tomove == black) result ^= Zobrist.tomove;
	
	return result;*/
	
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

TThashentry* lookup(const u64 position){
	const u64 tmp = position % TTableSize;
	if (TranspositionTable[tmp].pos == position){
		return &TranspositionTable[tmp];
	}
	return NULL;
}

void printTransTable(){
	for (int i = 0; i < TTableSize; i++){
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
	for (int i = 0; i < TTableSize; i++){
		TranspositionTable[i].pos = 0;
	}
}

#ifdef DEBUG
u64 Rcollision = 0;
u64 Rmatch = 0;
u64 Wcollision = 0;
u64 Wmatch = 0;
void printCollisionStats(){
	//printTransTable();
	printf("TTable reading:\n");
	printf("Collisions: %llu\n", Rcollision);
	printf("Matches: %llu\n", Rmatch);
	printf("Ratio to all: %lf\n\n", (double)Rmatch / (Rmatch + Rcollision + 1));
	//~ printf("Writing:\n");
	//~ printf("Collisions: %llu\n", Wcollision);
	//~ printf("Matches: %llu\n", Wmatch);
	//~ printf("Ratio to all: %lf\n\n", (double)Wmatch / (Wmatch + Wcollision + 1));
	
	u64 count = 0;
	for (int i = 0; i < TTableSize; i++){
		if (TranspositionTable[i].pos != 0){
			count++;
		}
	}
	printf("\nTTable: \n");
	printf("Filled: %llu\n", count);
	printf("All: %d\n", TTableSize);
	printf("Ratio to all: %lf\n", (double)count / TTableSize);
	
	Rcollision = Rmatch = 0;
	Wcollision = Wmatch = 0;
}
#endif

int readHashEntry(const u64 pos, int* alpha, int* beta, const int depth, const int maxdepth, const int oddity){
	return NO_HASH_ENTRY;
	TThashentry *current = &TranspositionTable[pos % TTableSize];
		
	if (current->pos != pos) {
		#ifdef DEBUG
		if (current->pos != 0) Rcollision++;
		#endif
		return NO_HASH_ENTRY;
	}
	
	if (current->depth <= maxdepth - depth){
		return NO_HASH_ENTRY;	
	}
	
	#ifdef DEBUG
	Rmatch++;
	#endif
	
	/* Pseudocode:
	    if ttEntry is valid and ttEntry.depth ≥ depth then
		if ttEntry.flag = EXACT then
			return ttEntry.value
		else if ttEntry.flag = LOWERBOUND then
			α := max(α, ttEntry.value)
		else if ttEntry.flag = UPPERBOUND then
			β := min(β, ttEntry.value)

		if α ≥ β then
			return ttEntry.value
	 * */
	
	int tempeval = current->eval; 
	if (tempeval >= whitewon){
		tempeval = tempeval - depth  - 1;
	}
	else if (tempeval <= blackwon){
		tempeval += depth  + 1;
	}
	
	if (oddity){	
		tempeval *= -1;
		switch (current->flag){
			case exactFlag:
				return tempeval;
			case lastBest:
				return tempeval;
			case alphaFlag:
				if (tempeval < -(*beta)) (*beta) = -tempeval;
				break;
			case betaFlag:
				if (tempeval > -(*alpha)) (*alpha) = -tempeval;
				break;
		}
		
		if (-(*alpha) < -(*beta)){
			return tempeval;
		}
	}
	else {
		switch (current->flag){
			case exactFlag:
				return tempeval;
			case lastBest:
				return tempeval;
			case alphaFlag:
				if (tempeval < *alpha) (*alpha) = tempeval;
				break;
			case betaFlag:
				if (tempeval > *beta) (*beta) = tempeval;
				break;
		}
		
		if (*alpha > *beta){
			return tempeval;
		}
	}

	return NO_HASH_ENTRY;
}


void storePosTT(const u64 pos, const int eval, const evalflag flag, const int depth, const int maxdepth){
	u64 current = pos % TTableSize;
	#ifdef DEBUG
	if (TranspositionTable[current].pos == 0) Wmatch++;
	else Wcollision++;
	#endif
	
	TranspositionTable[current].pos = pos; //key 
	TranspositionTable[current].eval = eval;
	TranspositionTable[current].flag = flag;
	TranspositionTable[current].depth = maxdepth - depth;
	//~ TranspositionTable[current].m = m;
	//~ TranspositionTable[current].next = next;
	
	
	//~ printf("tablesize: %ld\n", TTableSize);
	//~ printf("currenthash: \n");
	//~ printBitPieceAsBoard(pos);
	//~ printTransTable();
}

void printHashEntry(u64 pos){
	TThashentry* current = lookup(pos);
	if (current == NULL) {
		printf("No record/overweitten\n");
		return;
	}
	printf("%lf %s depth: %d ", current->eval * 0.01/* * (tomove == white ? 1: -1)*/, current->flag == exactFlag ? " exactFlag" : current->flag == lastBest ? " lastBest" : " otherFlag", current->depth);
	printBitPiece(pos);
}

//~ void rmBestMoveFlag(u64 pos){
	//~ TThashentry* current = lookup(pos);
	//~ if (current == NULL) {
		//~ return;
	//~ }
	//~ current->flag = exactFlag;
//~ }

static inline void swap(bitboard* a, bitboard* b) { 
	bitboard temp = *a;
	*a = *b; 
	*b = temp;
	//int x;
} 


static inline int getEval(u64 pos){
	TThashentry *current = &TranspositionTable[pos % TTableSize];
	if (current->pos == pos){
		if (current->flag == lastBest) {
			//remove the flag maybe?
			current->flag = exactFlag;
			return 1000000 + current->depth;
		}
		if (current->flag == exactFlag || current->flag == alphaFlag) return current->eval;
	}
	return -1000000;
}

/* order moves in a non-descending order based on the evals stored in the hash table */
/* simple insertion sort, too few elements to make something fancy */
void orderMoves(movearray* legalmoves){
	for (int i = 1; i < legalmoves->size; i++){
		for (int j = 0; j < legalmoves->size - i; j++){
			if (getEval(legalmoves->boards[j].hashValue) < getEval(legalmoves->boards[j+1].hashValue)) swap(&legalmoves->boards[j], &legalmoves->boards[j+1]);
		}
	}
}


/* 
 * REPETITION TABLE
 * */

u64 RepetitionTable[REPETITION_TABLE_SIZE];
int RTwriteIndex = 0;

/* The last capture, castling or pawn move marks the last position that could be
 * a repetition. When reading the moves from the input, we can reset the write 
 * index to 0, if the move was special. If a new move was searched, we increase
 * the write index, and if the move was taken back, we decrease the write index.
 * This way the new positions constantly get overwritten, while the older ones
 * are kept. If the search is cancelled, we will get new line of moves, so
 * it dowsn't matter if we lost track of the write index. 
 * 
 * The position is considered a draw after the FIRST repetition, so the engine
 * avoids previous positions like fire, if it think's it's better. */

bool isRepetition(const u64 pos){
	for (int i = (RTwriteIndex > REPETITION_TABLE_SIZE ? REPETITION_TABLE_SIZE : RTwriteIndex); i >= 0; i--){
		if (RepetitionTable[i] == pos)	return true;
	}
	
	return false;
}

void storeRepetiton(const u64 pos){
	RTwriteIndex++;
	
	if (RTwriteIndex < REPETITION_TABLE_SIZE){
		//store the position if there's room for it	
		RepetitionTable[RTwriteIndex] = pos;
	}
} 


void rmLastRepetition(){
	if (RTwriteIndex > 0) RTwriteIndex--;
}
