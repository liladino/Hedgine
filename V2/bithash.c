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
	if (TranspositionTable[position % TableSize].pos == position){
		return &TranspositionTable[position % TableSize];
	}
	return NULL;
}

void storePos(u64 pos, int eval, evalflag flag, int depth, move m){
	int current = pos % TableSize;
	TranspositionTable[current].pos = pos; //key and
	TranspositionTable[current].eval = eval;
	TranspositionTable[current].flag = flag;
	TranspositionTable[current].depth = depth;
	TranspositionTable[current].m = m;
}



void makeMoveOld(char board[12][12], move m){ //promotion: 0 = nincs, 'R', 'r', 'Q', 'q', ....
	squarenums from, to;
	char promotion = m.promotion;
	from = m.from;
	to = m.to;
	
	if (promotion == 0){
		if ( (board[from.rank][from.file] == 'K' || board[from.rank][from.file] == 'k') && abs(to.file - from.file) == 2){ 
			//sancolas
			if (to.file == 'c' - 'a' + 2){
				board[from.rank]['d' - 'a' + 2] = board[from.rank][2];
				board[from.rank][2] = ' '; //bastya helye
			}
			else{
				board[from.rank]['f' - 'a' + 2] = board[from.rank]['h' - 'a' + 2];
				board[from.rank]['h' - 'a' + 2] = ' ';
			}
		}
		if ((board[from.rank][from.file] == 'P' || board[from.rank][from.file] == 'p') && board[to.rank][to.file] == ' ' && abs(to.file-from.file) == 1){ 
			//en passant -> elmozdul a vonalrol => ut, de a mezo ahova erkezik ures
			board[from.rank][to.file] = ' '; //leutott gyalog
		}
		board[to.rank][to.file] = board[from.rank][from.file];
	}
	else{
		if (board[from.rank][from.file] == 'P')
			board[to.rank][to.file] = promotion;
		else
			board[to.rank][to.file] = promotion + 'a' -  'A';
	}
	board[from.rank][from.file] = ' ';
}

static void setMetaOld(char board[12][12], move m, int castling[], squarenums *enpass, bool *tomove){ 
	(*tomove) = !(*tomove);
	squarenums from, to;
	from = m.from;
	to = m.to;
	//ha a gyalog kettot ment, az en passant lehetoseg beallitasa
	int piece = board[from.rank][from.file];
	if (piece == 'P' || piece == 'p'){
		if  (abs(to.rank - from.rank) == 2){
			int enpasspiece;
			if (piece == 'P'){
				enpasspiece = 'p';
			}
			else{
				enpasspiece = 'P';
			}
			if ( board[to.rank][to.file - 1] == enpasspiece || board[to.rank][to.file + 1] == enpasspiece){ //csak akkor van en passant target square, ha van gyalog ami utne oda
				enpass->file = to.file;
				enpass->rank = (from.rank + to.rank)/2; 
			}
			else{
				enpass->file = -1;
				enpass->rank = -1;
			}
		}
		else{
			enpass->file = -1;
			enpass->rank = -1;
		}
	}
	else{
		enpass->file = -1;
		enpass->rank = -1;
	}
	
	//ha kiralylepes, sanc elvetele
	if (piece == 'K'){
		castling[0] = 0;
		castling[1] = 0;
	}
	if (piece == 'k'){
		castling[2] = 0;
		castling[3] = 0;
	}
	
	
	if ((from.rank == 2 && from.file == 9) || (to.rank == 2 && to.file == 9)){ //ha h1-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[0] = 0;
	}	
	if ((from.rank == 2 && from.file == 2) || (to.rank == 2 && to.file == 2)){ //ha a1-rol leptek, vagy oda leptek, vezeroldali sanc = 0
		castling[1] = 0;
	}
	if ((from.rank == 9 && from.file == 9) || (to.rank == 9 && to.file == 9)){ //ha h8-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[2] = 0;
	}
	if ((from.rank == 9 && from.file == 2) || (to.rank == 9 && to.file == 2)){ //ha h8-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[3] = 0;
	}
}

void printBestLine(char board[12][12], bool tomove, int castling[4], squarenums enpass){
	hashentry* current = lookup(boardConvert(board, castling, enpass, tomove).hashValue);
	while (current != NULL) {
		printmove(current->m);
		printf("%lf \n", current->eval * 0.01);
		makeMoveOld(board, current->m);
		setMetaOld(board, current->m, castling, &enpass, &tomove);
		current = lookup(boardConvert(board, castling, enpass, tomove).hashValue);
	}
}
/*
void clearTransTable(){
	lastelement = 0;
}

int main(){
	//printBitPiece(rand64());
	setHashKey();
	return 0;
}*/
