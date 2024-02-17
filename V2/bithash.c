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

#define TableSizeMB 64 
#define TableSize TableSizeMB * 1024 * 1024 / sizeof(hashentry)

hashentry TranspositionTable[TableSize];

hashentry* lookup(u64 position){
	for (int i = 0; i < lastelement; i++){
		if (TranspositionTable[i].position == position){
			return &TranspositionTable[i];
		}
	}
	return NULL;
}

void storePos(u64 pos, u64 nextpos, int eval, move m){
	int current = lastelement;
	if (lastelement == TableSize){
		//lastelement = 0;
		current = rand() % TableSize;
	}
	else lastelement++;
	TranspositionTable[current].position = pos;
	TranspositionTable[current].eval = eval;
	TranspositionTable[current].m = m;
}

void printBestLine(u64 pos){
	hashentry* current = lookup(pos);
	//while (current != NULL){
		printmove(current->m);
		printf("%lf \n", current->eval * 0.01);
		//current = lookup(current->next);
	//}
}

void clearTransTable(){
	lastelement = 0;
}
/*
int main(){
	//printBitPiece(rand64());
	setHashKey();
	return 0;
}*/
