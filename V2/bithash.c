#include "bithash.h"

#define TableSizeMB 1
const int TableSize = 			(TableSizeMB * 1024 * 1024 / sizeof(hashentry));
hashentry TranspositionTable[	(TableSizeMB * 1024 * 1024 / sizeof(hashentry))	];


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
#ifdef DEBUG 
	#define endl printf("\n")
	printf("%d MB, %d", TableSizeMB, TableSize);
	endl;
	/*for (int i = 0; i < 64; i++){
		for (int j = 0; j < 12; j++){
			printBitPiece(Zobrist.squares[i][j]);
		}
		endl;
	}
	endl;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 2; j++){
			printBitPiece(Zobrist.castlerights[i][j]);
		}
		endl;
	}
	endl;
	for (int i = 0; i < 8; i++){
		printBitPiece(Zobrist.enpassantfile[i]);
	}
	endl;
	printBitPiece(Zobrist.tomove);
	endl;*/
#endif
}

u64 hashPosition(const bitboard board, bool tomove){
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

hashentry* lookup(const u64 position){
	const u64 tmp = position % TableSize;
	if (TranspositionTable[tmp].pos == position){
		return &TranspositionTable[tmp];
	}
	return NULL;
}

void printTransTable(){
	for (int i = 0; i < TableSize; i++){
		if (TranspositionTable[i].pos != 0){
			printf("place: %lld\n", TranspositionTable[i].pos % (TableSize));
		/*	printf("hash: ");
			printBitPiece(TranspositionTable[i].pos);
			printf("eval: %d\n", TranspositionTable[i].eval);
			printf("nextboard: \n");
			printBitPiece(TranspositionTable[i].next);*/
		}
	}
}

void clearTransTable(){
	for (int i = 0; i < TableSize; i++){
		TranspositionTable[i].pos = 0;
	}
}


u64 Rcollision = 0;
u64 Rmatch = 0;
u64 Wcollision = 0;
u64 Wmatch = 0;
void printCollisionStats(){
	//printTransTable();
	//~ printf("Reading:\n");
	//~ printf("Collisions: %llu\n", Rcollision);
	//~ printf("Matches: %llu\n", Rmatch);
	//~ printf("Ratio to all: %lf\n\n", (double)Rmatch / (Rmatch + Rcollision > 0 ? Rmatch + Rcollision : 1));
	//~ printf("Writing:\n");
	//~ printf("Collisions: %llu\n", Wcollision);
	//~ printf("Matches: %llu\n", Wmatch);
	//~ printf("Ratio to all: %lf\n\n", (double)Wmatch / (Wmatch + Wcollision > 0 ? Wmatch + Wcollision : 1));
	
	u64 count = 0;
	for (int i = 0; i < TableSize; i++){
		if (TranspositionTable[i].pos != 0){
			count++;
		}
	}
	printf("\nTTable: \n");
	printf("Filled: %llu\n", count);
	printf("All: %d\n", TableSize);
	printf("Ratio to all: %lf\n", (double)count / TableSize);
	
	Rcollision = Rmatch = 0;
	Wcollision = Wmatch = 0;
}

int readHashEntry(const u64 pos, const int alpha, const int beta, const int depth){
	hashentry *current = &TranspositionTable[pos % TableSize];
	//hashentry *current = lookup(pos);
	//if (current == NULL) return NO_HASH_ENTRY;
	//~ return NO_HASH_ENTRY;
	
	if (current->pos == pos) {
		#ifdef DEBUG
		Rmatch++;
		#endif
		if (current->depth >= depth) {
			int temp = current->eval; 
			if (temp >= whitewon){
				temp = temp - depth /* - 1*/;
			}
			else if (temp <= blackwon){
				temp += depth /* + 1*/;
			}
			
			switch (current->flag){
				case exactFlag:
					return temp;
				case lastBest:
					return temp;
				case alphaFlag:
					if (temp <= alpha) return alpha;
					break;
				case betaFlag:
					if (temp >= beta) return beta;
					break;
			}
			
		}
	}
	#ifdef DEBUG
	else{
		if (current->pos != 0) Rcollision++;
	}
	#endif
	return NO_HASH_ENTRY;
}


//~ move readHashEntryMove(const u64 pos){
	//~ //assuming readHashEntry was already called, and we tested it is indeed the pos we want to see
	//~ hashentry *current = &TranspositionTable[pos % TableSize];
	//~ return current->m;
//~ }


void storePos(const u64 pos, const int eval, const evalflag flag, const int depth /*, const move m, const u64 next*/){
	u64 current = pos % TableSize;
	#ifdef DEBUG
	if (TranspositionTable[current].pos == 0) Wmatch++;
	else Wcollision++;
	#endif
	
	TranspositionTable[current].pos = pos; //key 
	TranspositionTable[current].eval = eval;
	TranspositionTable[current].flag = flag;
	TranspositionTable[current].depth = depth;
	//~ TranspositionTable[current].m = m;
	//~ TranspositionTable[current].next = next;
	
	
	//~ printf("tablesize: %ld\n", TableSize);
	//~ printf("currenthash: \n");
	//~ printBitPieceAsBoard(pos);
	//~ printTransTable();
}


//~ void printBestLine(u64 pos, bool tomove){
	//~ //printTransTable();
	//~ hashentry* current = lookup(pos);
	//~ int parity = (tomove == white ? 1 : -1);
	//~ int counter = 0; //if theres a collision, prevent loops
	//~ printf("Best line:\n");
	//~ while (current != NULL && current->m.from.file != -1 && counter < 20) { //no node, or noMove
		//~ printmove(current->m);
		//~ printf("%lf \n", current->eval * 0.01 * parity);
		//~ current = lookup(current->next);
		//~ counter++;
	//~ }
//~ }

void printHashEntry(u64 pos){
	hashentry* current = lookup(pos);
	if (current == NULL) {
		printf("No record/overweitten\n");
		return;
	}
	printf("%lf %s depth: %d ", current->eval * 0.01/* * (tomove == white ? 1: -1)*/, current->flag == exactFlag ? " exactFlag" : current->flag == lastBest ? " lastBest" : " otherFlag", current->depth);
	printBitPiece(pos);
}

void rmBestMoveFlag(u64 pos){
	hashentry* current = lookup(pos);
	if (current == NULL) {
		return;
	}
	current->flag = exactFlag;
}

static inline void swap(bitboard* a, bitboard* b) { 
	bitboard temp = *a;
	*a = *b; 
	*b = temp;
	//int x;
} 


static inline int getEval(u64 pos){
	hashentry *current = &TranspositionTable[pos % TableSize];
	if (current->pos == pos){
		if (current->flag == lastBest) return 1000000 + current->depth;
		if (current->flag == exactFlag || current->flag == alphaFlag) return current->eval;
	}
	return -1000000;
}

/*order moves in a non-descending order based on the evals stored in the hash table*/
/*simple insertion sort, too few elements to make something fancy*/
void orderMoves(move_array* legalmoves){
	for (int i = 1; i < legalmoves->size; i++){
		for (int j = 0; j < legalmoves->size - i; j++){
			if (getEval(legalmoves->boards[j].hashValue) < getEval(legalmoves->boards[j+1].hashValue)) swap(&legalmoves->boards[j], &legalmoves->boards[j+1]);
		}
	}
}


