#include "headers/legalmoves.h"

#define WKINGSIDE 1
#define WQUEENSIDE 2
#define BKINGSIDE 4
#define BQUEENSIDE 8

/* PAWN MOVE MASKS
 * 
 * WHITE perspective
 * 
 * bitmask for 2 square moves:
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 11111111
 * 00000000 = 0x000000000000FF00LL
 * 
 * bitmask for 1 square move:
 * 00000000
 * 00000000
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 00000000 = 0x0000FFFFFFFFFF00LL
 * 
 * WEST and EAST are mirrored in the bitboard, cos a1 is the lsb.
 * bitmask for taking west:
 * h8 00000000
 *    11111110
 *    11111110
 *    11111110
 *    11111110
 *    11111110
 *    11111110 a2
 *    00000000 = 0x00FEFEFEFEFEFE00LL
 * 
 * bitmask for taking east:
 * 00000000
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 00000000 = 0x007F7F7F7F7F7F00LL
 * 
 * bitmask for promotion:
 * 00000000
 * 11111111
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000 = 0x00FF000000000000LL
 * 
 * */
#define PAWN_TAKE_EAST     0x007F7F7F7F7F7F00LL
#define PAWN_TAKE_WEST     0x00FEFEFEFEFEFE00LL

#define WPAWN_MOVE_FORWARD 0x0000FFFFFFFFFF00LL
#define WPAWN_PROMOTE      0x00FF000000000000LL
#define WPAWN_HOME         0x000000000000FF00LL
#define WPAWN_ENP_TAKES    0x000000FF00000000LL

#define BPAWN_MOVE_FORWARD 0x00FFFFFFFFFF0000LL
#define BPAWN_PROMOTE      0x000000000000FF00LL
#define BPAWN_HOME         0x00FF000000000000LL

#include "magicvalues.txt"

/* pops the least significant bit and returns an u64 with that bit only */
static inline u64 pop_lsb(u64 *var) {
	u64 lsb = *var & -*var;  // Isolate LS1B
	*var &= (*var - 1);      // Clear LS1B
	return lsb;
}

bool bitInCheck(const bitboard* const board, bool tomove){
	//~ int coloffset = tomove == white ? bking : wking;
	//~ u64 kingbit = (tomove == white ? board->piece[wking] : board->piece[bking]);
		
	int kingIndex = __builtin_ctzll((tomove == white ? board->piece[wking] : board->piece[bking]));
	
	if (tomove == white){
		if (board->piece[bpawn] & wpawnTakes[kingIndex]) return true;
		if (board->piece[bknight] & knightMoves[kingIndex]) return true;
	}
	else{
		if (board->piece[wpawn] & bpawnTakes[kingIndex]) return true;
		if (board->piece[wknight] & knightMoves[kingIndex]) return true;
	}
		
	{
		u64 friendly = enemypieces(board, tomove);
		u64 enemy = enemypieces(board, !tomove);
		int coloffset = (tomove == white ? bking : wking);
		u64 occ = enemy | friendly;
		
		u64 rookDir = board->piece[wrook + coloffset] | board->piece[wqueen + coloffset];
		if (Rmagic(kingIndex, occ) & rookDir) return true;

		u64 bishopDir = board->piece[wbishop + coloffset] | board->piece[wqueen + coloffset];
		if (Bmagic(kingIndex, occ) & bishopDir) return true;
		
		//~ printBitPieceAsBoard(possiblemoves);
	}

	//king
	if (board->piece[(tomove == white ? bking : wking)] & kingAttacks[kingIndex]){
		/* the king attacks are symmetrical, so it's enought to check one 
		 * side; if the orther attacks the first, the first attacks the other 
		 * too. */ 
		 return true;
	}
	
	return false;
}

//obsolete with the mailbox
static inline void deletePiece(bitboard *board, const u64 movemask){
	//if a rook gets taken, remove the castlingrights for it
	if (board->piece[wrook] & movemask){
		if ((1LLU << 7) & movemask) board->castlerights &= ~WKINGSIDE;
		else {if (1LLU & movemask) board->castlerights &= ~WQUEENSIDE;}
	}
	else if (board->piece[brook] & movemask){
		if ((1LLU << (7 + nort * 7)) & movemask) board->castlerights &= ~BKINGSIDE;
		else {if ((1LLU << (nort * 7)) & movemask) board->castlerights &= ~BQUEENSIDE;}
	}
	
	u64 inverse = (~movemask);
	
	board->piece[1] &= inverse;
	board->piece[2] &= inverse;
	board->piece[3] &= inverse;
	board->piece[4] &= inverse;
	board->piece[5] &= inverse;
	
	board->piece[7] &= inverse;
	board->piece[8] &= inverse;
	board->piece[9] &= inverse;
	board->piece[10] &= inverse;
	board->piece[11] &= inverse;
}

static inline void setcastlingrights(bitboard *board, bool tomove, u64 piece){
	/* check if we moved the rook from the corner, and if so, delete the 
	 * castling rights for that side. */
	if (tomove == white){
		if (piece & 1LL){
			//hashCastleO(board, 1);
			board->castlerights &= ~(WQUEENSIDE);
		}
		else if (piece & (1LLU << 7)){
			//hashCastleO(board, 0);
			board->castlerights &= ~(WKINGSIDE);
		}
	}
	else {
		if (piece & (1LLU << (7 * nort))){
			//hashCastleO(board, 3);
			board->castlerights &= ~(BQUEENSIDE);
		}
		if (piece & (1LLU << (7 + 7 * nort))){
			//hashCastleO(board, 2);
			board->castlerights &= ~(BKINGSIDE);
		}
	}
}

static void addMoves(movearray* moves, int i, bool onlyCaptures, u64 enemy, u64 friendly, uint8_t piece, u64 possiblemoves){
	bitMove* legalmoves = moves->array;
	int* array_index = &(moves->size);
	
	if (onlyCaptures){
		u64 possiblecaptures = possiblemoves & enemy;
		
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			
			bitMove m = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			legalmoves[(*array_index)++] = m;			
		}
	}
	else {
		possiblemoves &= ~(enemy | friendly);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			bitMove m = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, 0};
			legalmoves[(*array_index)++] = m;
		}
	}
}

static void addBitKnightMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = knightMoves[i];
	uint8_t piece = (tomove != white ? bknight : wknight);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}


static void addBitBishopMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = Bmagic(i, enemy | friendly); 
	uint8_t piece = (tomove != white ? bbishop : wbishop);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}

/* Castling rights removal with rook: (must implement it elsewhere)
	#define SQUARE_A1 (1LLU)
	#define SQUARE_H1 (1LLU << 7)
	#define SQUARE_A8 (1LLU << (nort * 7))
	#define SQUARE_H8 (1LLU << (7 + nort * 7))
	if (!bitInCheck(&board, tomove)) {
		if (copy.piece[wrook] & piece){
			if ((1LLU << 7) & piece) board.castlerights &= ~WKINGSIDE;
			else if (1LLU & piece)   board.castlerights &= ~WQUEENSIDE;
		}
		else if (copy.piece[brook] & piece){
			if ((1LLU << (7 + nort * 7)) & piece)  board.castlerights &= ~BKINGSIDE;
			else if ((1LLU << (nort * 7)) & piece) board.castlerights &= ~BQUEENSIDE;
		}
		
		legalmoves[(*array_index)++] = board;
	}
	*/
	
static void addBitRookMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = Rmagic(i, enemy | friendly); 
	uint8_t piece = (tomove != white ? brook : wrook);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}

static void addBitQueenMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = (Bmagic(i, enemy | friendly)) | (Rmagic(i, enemy | friendly)); 
	uint8_t piece = (tomove != white ? bqueen : wqueen);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}

static void addBitKingMoves(movearray* moves, const bitboard* const board, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitMove* legalmoves = moves->array;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = kingAttacks[i];
	uint8_t piece = (tomove != white ? bking : wking);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
	
	if (onlyCaptures){
		//add castling too, like if it was a capture 
		if (tomove == white && i == 4) {
			if ((board->castlerights & WKINGSIDE)) {
				legalmoves[(*array_index)++] = (bitMove){i, 6, piece, -1, CASTLE_FLAG};
			}
			
			if ((board->castlerights & WQUEENSIDE)) {
				legalmoves[(*array_index)++] = (bitMove){i, 2, piece, -1, CASTLE_FLAG};
			}
		}
		else if (tomove == black && i == 60) {
			if ((board->castlerights & BKINGSIDE)) {
				legalmoves[(*array_index)++] = (bitMove){i, 62, piece, -1, CASTLE_FLAG};
			}
			
			if ((board->castlerights & BQUEENSIDE)) {
				legalmoves[(*array_index)++] = (bitMove){i, 58, piece, -1, CASTLE_FLAG};
			}
		}
	}
}

//~ (movearray* moves, const bitboard* const board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	//~ bitboard copy = board;
	//~ int coloffset = wking + (tomove != white ? bking : wking);
	
	//~ bitboard* legalmoves = moves->boards;
	//~ int* array_index = &(moves->size);
	
	//~ #define ADDKINGMOVE() do { board.enpassanttarget = 0; if (!bitInCheck(&board, tomove)) { legalmoves[(*array_index)++] = board; } board = copy; } while(0);
	
	//~ if (onlyCaptures){
		//~ /* these should be set when we trz to take east or west, so that the
		 //~ * castling can know, if it will travel through a check */ 
		//~ bool kingSideCastle = false, queenSideCastle = false;
		
		//~ //set castle possibility shit
		//~ if (((tomove == white && i == 4) || (tomove == black && i == 60)) && (!bitInCheck(&board, tomove))){
			//~ if ((((piece >> west) | (piece >> 2 * west) | (piece >> 3 * west)) & (friendly | enemy)) == 0){
				//~ //nothing is there, check if its in check, BUT DO NOT ADD THE MOVE
				//~ board.piece[coloffset] = piece >> west;
				//~ board.enpassanttarget = 0; 
				//~ if (!bitInCheck(&board, tomove)) { 
					//~ queenSideCastle = true;
				//~ } 
				//~ board = copy;
			//~ }
			//~ if ((((piece << east) | (piece << 2 * east)) & (friendly | enemy)) == 0){
				//~ board.piece[coloffset] = piece << east;
				//~ board.enpassanttarget = 0; 
				//~ if (!bitInCheck(&board, tomove)) { 
					//~ kingSideCastle = true;
				//~ } 
				//~ board = copy;
			//~ }			
		//~ }
		
		//~ //taking
		//~ u64 possiblemoves = kingAttacks[i] & enemy;
	
		//~ while (possiblemoves) { 
			//~ u64 currentmove = pop_lsb(&possiblemoves); 
			
			//~ board.piece[coloffset] = currentmove; //move the king bit
			//~ board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
			//~ deletePiece(&board, currentmove);
			//~ ADDKINGMOVE();
		//~ }
		
		//~ //castling is not a capture, but its good to look for early on
		//~ if (tomove == white && i == 4){
			//~ if (kingSideCastle && board.castlerights & WKINGSIDE){
				//~ board.piece[wking] = piece << (east * 2); //move the king bit
				//~ board.piece[wrook] ^= (piece << east) | (piece << (east * 3)); //move the rook bit 
				//~ board.castlerights &= 12; //delete white castling rights
				//~ ADDKINGMOVE();
			//~ }
			
			//~ if (queenSideCastle && board.castlerights & WQUEENSIDE){
				//~ board.piece[wking] = piece >> (west * 2); //move the king bit
				//~ board.piece[wrook] ^= (piece >> west) | (piece >> (west * 4)); //move the rook bit
				//~ board.castlerights &= 12; //delete white castling rights
				//~ ADDKINGMOVE();
			//~ }
		//~ }
		//~ if (tomove == black && i == 60){
			//~ if (kingSideCastle && board.castlerights & BKINGSIDE){
				//~ board.piece[bking] = piece << (east * 2); //move the king bit
				//~ board.piece[brook] ^= (piece << east) | (piece << (east * 3)); //move the rook bit
				//~ board.castlerights &= 3; //delete black castling rights
				//~ ADDKINGMOVE();
			//~ }
			
			//~ if (queenSideCastle && board.castlerights & BQUEENSIDE){
				//~ board.piece[bking] = (piece >> (west * 2)); //move the king bit
				//~ board.piece[brook] ^= (piece >> west) | (piece >> (west * 4)); //move the rook bit
				//~ board.castlerights &= 3; //delete black castling rights
				//~ ADDKINGMOVE();
			//~ }
		//~ }
	//~ }
	//~ else{
		//~ u64 possiblemoves = kingAttacks[i] & ~(enemy | friendly);
	
		//~ while (possiblemoves) { 
			//~ u64 currentmove = pop_lsb(&possiblemoves); 
			
			//~ board.piece[coloffset] = currentmove; //move the king bit
			//~ board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
			//~ ADDKINGMOVE();
		//~ }
	//~ }
//~ }

static void addBitPawnMoveWhite(movearray* moves, const bitboard* const board, u64 piecemask, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitMove* legalmoves = moves->array;
		
	u64 moveforward = piecemask << 8;
	uint8_t piece = wpawn;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && (piecemask & WPAWN_MOVE_FORWARD) && !(moveforward & (enemy | friendly))){
		legalmoves[(*array_index)++] = (bitMove){i, i+8, piece, -1, 0};
		
		if ((piecemask & WPAWN_HOME) && 0 == ((piecemask << nort*2) & (enemy | friendly))){
			legalmoves[(*array_index)++] = (bitMove){i, i+16, piece, -1, DOUBLE_PAWNMOVE_FLAG};
		}
	}
	else if (!onlyCaptures){
		return;
	}
	
		
	if (onlyCaptures){
		/* Takes / promotion */
		u64 possiblecaptures = wpawnTakes[i];
			
		while (possiblecaptures) { 	
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piecemask & WPAWN_MOVE_FORWARD)){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			}
			else if (board->enpassanttarget == currentmove){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG & EN_PASSANT_FLAG};
			}
			else if ((currentmove & enemy) && (piecemask & WPAWN_PROMOTE)) {				
				for (int prom = wqueen; prom <= wknight; prom++){
					legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, prom, CAPTURE_FLAG & PROMOTION_FLAG};
				}
			}
		}
		
		if ((piecemask & WPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			for (int prom = wqueen; prom <= wknight; prom++){
				legalmoves[(*array_index)++] = (bitMove){i, i+8, piece, prom, PROMOTION_FLAG};
			}
		}
	}
}

static void addBitPawnMoveBlack(movearray* moves, const bitboard* const board, u64 piecemask, size_t i, bool onlyCaptures, u64 enemy, u64 friendly){
	int *array_index = &(moves->size);
	bitMove* legalmoves = moves->array;
	
	u64 moveforward = piecemask >> 8;
	uint8_t piece = bpawn;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && (piecemask & BPAWN_MOVE_FORWARD) && !(moveforward & (enemy | friendly))){
		legalmoves[(*array_index)++] = (bitMove){i, i-8, piece, -1, 0};
		
		if ((piecemask & BPAWN_HOME) && 0 == ((piecemask >> sout*2) & (enemy | friendly))){
			legalmoves[(*array_index)++] = (bitMove){i, i-16, piece, -1, DOUBLE_PAWNMOVE_FLAG};
		}
	}
	else if (!onlyCaptures){
		return;
	}
	
		
	if (onlyCaptures){
		/* Takes / promotion */
		u64 possiblecaptures = bpawnTakes[i];
			
		while (possiblecaptures) { 	
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piecemask & BPAWN_MOVE_FORWARD)){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			}
			else if (board->enpassanttarget == currentmove){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG & EN_PASSANT_FLAG};
			}
			else if ((currentmove & enemy) && (piecemask & BPAWN_PROMOTE)) {				
				for (int prom = bqueen; prom <= bknight; prom++){
					legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, prom, CAPTURE_FLAG & PROMOTION_FLAG};
				}
			}
		}
		
		if ((piecemask & BPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			for (int prom = bqueen; prom <= bknight; prom++){
				legalmoves[(*array_index)++] = (bitMove){i, i-8, piece, prom, PROMOTION_FLAG};
			}
		}
	}
}

void bitGenerateLegalmoves(movearray* moves, const bitboard* const board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	moves->size = 0; //delete anything that's there
	
	int coloffset = (tomove != white ? 6 : 0);
	
	movearray quiets;
	quiets.size = 0;
	
	u64 pieces = board->piece[wpawn + coloffset];
	if (tomove == black){
		while (pieces) {
			u64 mask = pop_lsb(&pieces);
			addBitPawnMoveBlack(moves, board, mask, __builtin_ctzll(mask), true, enemy, friendly);
			if (!onlyCaptures) addBitPawnMoveBlack(&quiets, board, mask, __builtin_ctzll(mask), false, enemy, friendly);
		}
	}
	else {
		while (pieces) {
			u64 mask = pop_lsb(&pieces);
			addBitPawnMoveWhite(moves, board, mask, __builtin_ctzll(mask), true, enemy, friendly);
			if (!onlyCaptures) addBitPawnMoveWhite(&quiets, board, mask, __builtin_ctzll(mask), false, enemy, friendly);
		}
	}
	pieces = board->piece[wqueen + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitQueenMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitQueenMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wbishop + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitBishopMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitBishopMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wknight + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitKnightMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitKnightMoves(&quiets, tomove,  __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wrook + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitRookMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitRookMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wking + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitKingMoves(moves, board, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitKingMoves(&quiets, board, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	
	// add quiet moves
	for (int i = 0; i < quiets.size; i++){
		moves->array[moves->size ++] = quiets.array[i]; 
	}
	
	//~ printf("--%d--\n--%d--\n", sizeof(bitboard), sizeof(movearray));
	
	/* after these loops are done, we should iterate over the new legal 
	 * moves, and generate a hash for each */
	//~ for (int i = 0; i < moves->size; i++){
		//~ moves->boards[i].hashValue = hashPosition(&(moves->boards[i]), tomove);
	//~ }
}

resultconst gameend(const bitboard* const board, bool tomove){
	movearray moves;
	
	bitGenerateLegalmoves(&moves, board, tomove, true);
	if (moves.size != 0) return ONGOING;
	bitGenerateLegalmoves(&moves, board, tomove, false);
	if (moves.size != 0) return ONGOING;
	
	if (bitInCheck(board, tomove)) return (tomove == white ? BLACKWON : WHITEWON);
	return DRAW;
}
