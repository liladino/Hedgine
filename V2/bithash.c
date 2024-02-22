#include "bithash.h"

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

u64 hashPosition(bitboard board, bool tomove){
	u64 result = 0;
	u64 mask = 1;
	for (int i = 0; i < 64; i++){
		for (int j = 0; j < 12; j++){
			if (board.piece[j] & mask){
				result ^= Zobrist.squares[i][j];
				break;
			}
		}
		mask = mask << 1;
	}
	if (board.enpassanttarget){
		mask = 0x0101010101010101LL; //a file
		for (int i = 0; i < 8; i++){
			if (mask & board.enpassanttarget){
				result ^= Zobrist.enpassantfile[i];
				break;
			}
			mask = mask << 1;
		}
	}
	//else result ^= Zobrist.enpassantfile[8];
	mask = 1; //K
	for (int i = 0; i < 4; i++){
		if (board.castlerights & mask) result ^= Zobrist.castlerights[i][1];
		else result ^= Zobrist.castlerights[i][0];
		
		mask = mask << 1;
	}
	if (tomove == black) result ^= Zobrist.tomove;
	
	return result;
}

#define TableSizeMB 1
const int TableSize = (TableSizeMB * 1024 * 1024 / sizeof(hashentry));

hashentry TranspositionTable[(TableSizeMB * 1024 * 1024 / sizeof(hashentry))];

hashentry* lookup(const u64 position){
	if (TranspositionTable[position % TableSize].pos == position){
		return &TranspositionTable[position % TableSize];
	}
	return NULL;
}


int readHashEntry(const u64 pos, const int alpha, const int beta, const int depth){
	hashentry *current = &TranspositionTable[pos % TableSize];
	//hashentry *current = lookup(pos);
	//if (current == NULL) return NO_HASH_ENTRY;
	if (current->pos == pos) {
		if (current->depth >= depth) {
			switch (current->flag){
				case exactFlag:
					return current->eval;
				case alphaFlag:
					if (current->eval <= alpha) return alpha;
					break;
				case betaFlag:
					if (current->eval >= beta) return beta;
					break;
			}
			//if (hash_entry->flag == hash_flag_exact) return hash_entry->score;
			//if ((hash_entry->flag == hash_flag_alpha) && (hash_entry->score <= alpha))
			//if ((hash_entry->flag == hash_flag_beta) && (hash_entry->score >= beta)) 
		}
	}
	return NO_HASH_ENTRY;
}

move readHashEntryMove(const u64 pos){
	//assuming readHashEntry was already called, and we tested it is indeed the pos we want to see
	hashentry *current = &TranspositionTable[pos % TableSize];
	return current->m;
}

#ifdef DEBUG
void printTransTable(){
	for (int i = 0; i < TableSize; i++){
		if (TranspositionTable[i].pos != 0){
			printf("place: %lld\n", TranspositionTable[i].pos % (TableSize));
			printf("hash: ");
			printBitPiece(TranspositionTable[i].pos);
			printf("eval: %d\n", TranspositionTable[i].eval);
			printf("nextboard: \n");
			printBitPiece(TranspositionTable[i].next);
		}
	}
}
#endif

void storePos(u64 pos, int eval, evalflag flag, int depth, move m, u64 next){
	int current = pos % TableSize;
	TranspositionTable[current].pos = pos; //key 
	TranspositionTable[current].eval = eval;
	TranspositionTable[current].flag = flag;
	TranspositionTable[current].depth = depth;
	TranspositionTable[current].m = m;
	TranspositionTable[current].next = next;
	//printf("tablesize: %ld\n", TableSize);
	//printf("currenthash: \n");
	//printBitPieceAsBoard(pos);
	//printTransTable();
}


void printBestLine(u64 pos, bool tomove){
	//printTransTable();
	hashentry* current = lookup(pos);
	int parity = (tomove == white ? 1 : -1);
	int counter = 0; //if theres a collision, prevent loops
	while (current != NULL && current->m.from.file != -1 && counter < 20) { //no node, or noMove
		printmove(current->m);
		printf("%lf \n", current->eval * 0.01 * parity);
		current = lookup(current->next);
		counter++;
	}
}

void printHashEntry(u64 pos, bool tomove){
	hashentry* current = lookup(pos);
	if (current == NULL) {
		printf("No record/overweitten\n");
		return;
	}
	printf("%lf %d depth: %d ", current->eval * 0.01/* * (tomove == white ? 1: -1)*/, current->flag, current->depth);
	printBitPiece(pos);
}



static inline void swap(bitboard* a, bitboard* b) { 
	bitboard temp = *a;
	*a = *b; 
	*b = temp;
	//int x;
} 


static inline int getEval(u64 pos, bool tomove){
	hashentry *current = &TranspositionTable[pos % TableSize];
	if (current->pos == pos && current->flag == exactFlag){
		return current->eval /* (tomove == black ? 1 : -1)*/;
	}
	return -1000000;
}

/*order moves in a non-descending order based on the evals stored in the hash table*/
/*simple insertion sort, too few elements to make something fancy*/
void orderMoves(move_array* legalmoves, bool tomove, int depth){
	for (int i = 1; i < legalmoves->size; i++){
		for (int j = 0; j < legalmoves->size - i; j++){
			if (getEval(legalmoves->boards[j].hashValue, tomove) < getEval(legalmoves->boards[j+1].hashValue, tomove)) swap(&legalmoves->boards[j], &legalmoves->boards[j+1]);
		}
	}
}


