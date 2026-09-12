#include "legalmoves.h"

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
	
	u64 piecemask = 1;
	if (tomove == white){
		piecemask = piecemask << kingIndex;
		
		if (board->piece[bpawn] & wpawnTakes[kingIndex]) return true;
		if (board->piece[bknight] & knightMoves[kingIndex]) return true;
	}
	else{
		piecemask = piecemask << kingIndex;
		
		if (board->piece[wpawn] & bpawnTakes[kingIndex]) return true;
		if (board->piece[wknight] & knightMoves[kingIndex]) return true;
	}
		
	{
		u64 friendly = enemypieces(board, tomove);
		u64 enemy = enemypieces(board, !tomove);
		
		int coloffset = (tomove == white ? bking : wking);
		u64 possiblemoves = Rmagic(kingIndex, enemy | friendly) & enemy;
		
		//~ printBitPieceAsBoard(possiblemoves);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			if ((currentmove & board->piece[wqueen + coloffset]) || (currentmove & board->piece[wrook + coloffset])){
				return true;
			}
		
		}
		
		possiblemoves = Bmagic(kingIndex, enemy | friendly) & enemy;		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			if ((currentmove & board->piece[wqueen + coloffset]) || (currentmove & board->piece[wbishop + coloffset])){
				return true;
			}
		}
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
	//dont need to delete the king, it can never get taken
	//~ board->piece[0] &= inverse; 
	board->piece[1] &= inverse;
	board->piece[2] &= inverse;
	board->piece[3] &= inverse;
	board->piece[4] &= inverse;
	board->piece[5] &= inverse;
	//~ board->piece[6] &= inverse;
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

static void addBitKnightMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int index, bool onlyCaptures, u64 enemy, u64 friendly){	
	bitboard copy = board;
	int coloffset = wknight + (tomove != white ? bking : wking);
	u64 withoutpiece = board.piece[coloffset] & (~piece);
	
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = knightMoves[index];
	
	while (possiblemoves) { 
		u64 currentmove = pop_lsb(&possiblemoves); 
		if (onlyCaptures && (enemy & currentmove)){
			deletePiece(&board, currentmove);
			board.piece[coloffset] = withoutpiece | currentmove; 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
		else if (!onlyCaptures && 0 == (enemy & currentmove) && 0 == (friendly & currentmove)){
			board.piece[coloffset] = withoutpiece | currentmove; 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
	}
}

static void addBitKingMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitboard copy = board;
	//~ u64 withoutpiece = board.piece[wpawn] & (~piece);
	int coloffset = wking + (tomove != white ? bking : wking);
	
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	#define ADDKINGMOVE() do { board.enpassanttarget = 0; if (!bitInCheck(&board, tomove)) { legalmoves[(*array_index)++] = board; } board = copy; } while(0);
	
	if (onlyCaptures){
		/* these should be set when we trz to take east or west, so that the
		 * castling can know, if it will travel through a check */ 
		bool kingSideCastle = false, queenSideCastle = false;
		
		//set castle possibility shit
		if (((tomove == white && i == 4) || (tomove == black && i == 60)) && (!bitInCheck(&board, tomove))){
			if ((((piece >> west) | (piece >> 2 * west) | (piece >> 3 * west)) & (friendly | enemy)) == 0){
				//nothing is there, check if its in check, BUT DO NOT ADD THE MOVE
				board.piece[coloffset] = piece >> west;
				board.enpassanttarget = 0; 
				if (!bitInCheck(&board, tomove)) { 
					queenSideCastle = true;
				} 
				board = copy;
			}
			if ((((piece << east) | (piece << 2 * east)) & (friendly | enemy)) == 0){
				board.piece[coloffset] = piece << east;
				board.enpassanttarget = 0; 
				if (!bitInCheck(&board, tomove)) { 
					kingSideCastle = true;
				} 
				board = copy;
			}			
		}
		
		//taking
		u64 possiblemoves = kingAttacks[i] & enemy;
	
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			board.piece[coloffset] = currentmove; //move the king bit
			board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
			deletePiece(&board, currentmove);
			ADDKINGMOVE();
		}
		
		//castling is not a capture, but its good to look for early on
		if (tomove == white && i == 4){
			if (kingSideCastle && board.castlerights & WKINGSIDE){
				board.piece[wking] = piece << (east * 2); //move the king bit
				board.piece[wrook] ^= (piece << east) | (piece << (east * 3)); //move the rook bit 
				board.castlerights &= 12; //delete white castling rights
				ADDKINGMOVE();
			}
			
			if (queenSideCastle && board.castlerights & WQUEENSIDE){
				board.piece[wking] = piece >> (west * 2); //move the king bit
				board.piece[wrook] ^= (piece >> west) | (piece >> (west * 4)); //move the rook bit
				board.castlerights &= 12; //delete white castling rights
				ADDKINGMOVE();
			}
		}
		if (tomove == black && i == 60){
			if (kingSideCastle && board.castlerights & BKINGSIDE){
				board.piece[bking] = piece << (east * 2); //move the king bit
				board.piece[brook] ^= (piece << east) | (piece << (east * 3)); //move the rook bit
				board.castlerights &= 3; //delete black castling rights
				ADDKINGMOVE();
			}
			
			if (queenSideCastle && board.castlerights & BQUEENSIDE){
				board.piece[bking] = (piece >> (west * 2)); //move the king bit
				board.piece[brook] ^= (piece >> west) | (piece >> (west * 4)); //move the rook bit
				board.castlerights &= 3; //delete black castling rights
				ADDKINGMOVE();
			}
		}
	}
	else{
		u64 possiblemoves = kingAttacks[i] & ~(enemy | friendly);
	
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			board.piece[coloffset] = currentmove; //move the king bit
			board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
			ADDKINGMOVE();
		}
	}
}

static void addBitPawnMoveWhite(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitboard* legalmoves = moves->boards;
	
	u64 withoutpiece = board.piece[wpawn] & (~piece);
	
	u64 moveforward = piece << 8 /*, takewest = piece << noWe, takeeast = piece << noEa*/;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && piece & WPAWN_MOVE_FORWARD && !(moveforward & (enemy | friendly))){
		//moving forward 1 sqyare
		board.piece[wpawn] = withoutpiece | moveforward; 
		board.enpassanttarget = 0;
		
		if (!bitInCheck(&board, tomove)) {
			legalmoves[(*array_index)++] = board;	
		}
		
		if ((piece & WPAWN_HOME) && 0 == ((piece << nort*2) & (enemy | friendly))){
			//moving 2 squares
			board.piece[wpawn] = withoutpiece | (piece << 16); 
			if ( ((PAWN_TAKE_EAST & piece) && ((piece << (noEa + nort)) & board.piece[bpawn]))
			  || ((PAWN_TAKE_WEST & piece) && ((piece << (noWe + nort)) & board.piece[bpawn])) ){
				board.enpassanttarget = moveforward;
			}
			else {
				board.enpassanttarget = 0;
			}
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
		}
	}
	else if (!onlyCaptures){
		return;
	}
	
		
	if (onlyCaptures){
		bitboard copy = board;
		
		/* Takes */
		u64 possiblecaptures = wpawnTakes[i];
	
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piece & WPAWN_MOVE_FORWARD)){
				/* Takes normally */ 
				deletePiece(&board, currentmove);
				board.piece[wpawn] = (withoutpiece | currentmove); //move the pawn
				board.enpassanttarget = 0; 
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				board = copy;
			}
			else if (board.enpassanttarget == currentmove){
				/* En passant */
				board.piece[wpawn] = (withoutpiece | currentmove); //move the pawn
				board.piece[bpawn] = board.piece[bpawn] & ~(currentmove >> sout); //delete enemy pawn
				board.enpassanttarget = 0;
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				board = copy;
			}
			else if ((currentmove & enemy) && (piece & WPAWN_PROMOTE)) {
				/* Promotes AND takes */
				deletePiece(&board, currentmove);
				board.piece[wpawn] = withoutpiece;
				board.enpassanttarget = 0;
				
				for (int i = wqueen; i <= wknight; i++){
					board.piece[i] |= currentmove;
					
					if (!bitInCheck(&board, tomove)) {
						legalmoves[(*array_index)++] = board;
					}
					else {
						/* If one capture  results in we being in check, that 
						 * means no matter what piece will be there, the check 
						 * will remain. */
						break;
					}
					
					board.piece[i] &= ~currentmove;
				}
				board = copy;
			}
		}
		
		/* Promotes AND DOES NOT take */
		if ((piece & WPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			board.piece[wpawn] = withoutpiece;
			board.enpassanttarget = 0;
			
			for (int i = wqueen; i <= wknight; i++){
				board.piece[i] |= moveforward;
				
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				else {
					/* If one capture  results in we being in check, that 
					 * means no matter what piece will be there, the check 
					 * will remain. */
					break;
				}
				
				
				board.piece[i] &= ~moveforward;
			}
			board = copy;
		}
	}
}

static void addBitPawnMoveBlack(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitboard* legalmoves = moves->boards;
	
	u64 withoutpiece = board.piece[bpawn] & (~piece);
	
	u64 moveforward = piece >> 8 /*, takewest = piece >> soWe, takeeast = piece >> soEa*/;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && piece & BPAWN_MOVE_FORWARD && !(moveforward & (enemy | friendly))){
		//moving forward 1 sqyare
		board.piece[bpawn] = withoutpiece | moveforward; 
		board.enpassanttarget = 0;
		
		if (!bitInCheck(&board, tomove)) {
			legalmoves[(*array_index)++] = board;
		}	
		if ((piece & BPAWN_HOME) && 0 == ((piece >> sout*2) & (enemy | friendly))){
			//moving 2 squares
			board.piece[bpawn] = withoutpiece | (piece >> 16); 
			if ( ((PAWN_TAKE_EAST & piece) && ((piece >> (soEa + sout)) & board.piece[wpawn]))
			  || ((PAWN_TAKE_WEST & piece) && ((piece >> (soWe + sout)) & board.piece[wpawn])) ){
				board.enpassanttarget = moveforward;
			}
			else {
				board.enpassanttarget = 0;
			}
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
		}
	}
	else if (!onlyCaptures){
		return;
	}
	
		
	if (onlyCaptures){
		bitboard copy = board;
		
		/* Takes */
		u64 possiblecaptures = bpawnTakes[i];
	
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piece & BPAWN_MOVE_FORWARD)){
				/* Takes normally */
				deletePiece(&board, currentmove);
				board.piece[bpawn] = (withoutpiece | currentmove); //move the pawn
				board.enpassanttarget = 0;
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				board = copy;
			}
			else if (board.enpassanttarget == currentmove){
				/* En passant */
				board.piece[bpawn] = (withoutpiece | currentmove); //move the pawn
				board.piece[wpawn] = board.piece[wpawn] & ~(currentmove << nort); //delete enemy pawn
				board.enpassanttarget = 0;
				
				//~ printBitBoard2d(stdout, board);
				
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				board = copy;
			}
			else if ((currentmove & enemy) && (piece & BPAWN_PROMOTE)) {
				/* Promotes AND takes */
				deletePiece(&board, currentmove);
				board.piece[bpawn] = withoutpiece;
				board.enpassanttarget = 0;
				
				for (int i = bqueen; i <= bknight; i++){
					board.piece[i] |= currentmove;
					
					if (!bitInCheck(&board, tomove)) {
						legalmoves[(*array_index)++] = board;
					}
					else {
						/* If one capture  results in we being in check, that 
						 * means no matter what piece will be there, the check 
						 * will remain. */
						break;
					}
					
					board.piece[i] &= ~currentmove;
				}
				board = copy;
			}
		}
		
		/* Promotes AND DOES NOT take */
		if ((piece & BPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			board.piece[bpawn] = withoutpiece;
			board.enpassanttarget = 0;
			
			for (int i = bqueen; i <= bknight; i++){
				board.piece[i] |= moveforward;
				
				if (!bitInCheck(&board, tomove)) {
					legalmoves[(*array_index)++] = board;
				}
				else {
					/* If one capture  results in we being in check, that 
					 * means no matter what piece will be there, the check 
					 * will remain. */
					break;
				}
				
				board.piece[i] &= ~moveforward;
			}
			board = copy;
		}
	}
}

static void addBitBishopMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = Bmagic(i, enemy | friendly); 
	
	int coloffset = wbishop + (tomove != white ? bking : wking);
	u64 withoutpiece = board.piece[coloffset] & (~piece);
	
	bitboard copy = board;
	
	if (onlyCaptures){
		u64 possiblecaptures = possiblemoves & enemy;
		
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			
			deletePiece(&board, currentmove);
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
	}
	else {
		possiblemoves &= ~(enemy | friendly);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
	}
}

static void addBitRookMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = Rmagic(i, enemy | friendly); 
	
	int coloffset = wrook + (tomove != white ? bking : wking);
	u64 withoutpiece = board.piece[coloffset] & (~piece);
	
	bitboard copy = board;
	
	if (onlyCaptures){
		u64 possiblecaptures = possiblemoves & enemy;
		
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			
			deletePiece(&board, currentmove);
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
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
			board = copy;
		}
	}
	else {
		possiblemoves &= ~(enemy | friendly);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
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
			board = copy;
		}
	}
}

static void addBitQueenMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = (Bmagic(i, enemy | friendly)) | (Rmagic(i, enemy | friendly)); 
	
	int coloffset = wqueen + (tomove != white ? bking : wking);
	u64 withoutpiece = board.piece[coloffset] & (~piece);
	
	bitboard copy = board;
	
	if (onlyCaptures){
		u64 possiblecaptures = possiblemoves & enemy;
		
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			
			deletePiece(&board, currentmove);
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
	}
	else {
		possiblemoves &= ~(enemy | friendly);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			board.piece[coloffset] = (withoutpiece | currentmove); 
			board.enpassanttarget = 0;
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
	}
}

void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(&board, !tomove); 
	u64 friendly = enemypieces(&board, tomove);
	
	moves->size = 0; //delete anything that's there
		
	if (tomove == black){
		/* if we search the moves for black, start with the moves close to the
		 * white backrank.*/
		u64 mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask)        {addBitPawnMoveBlack(moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[brook] & mask)   {addBitRookMoves    (moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[bbishop] & mask) {addBitBishopMoves  (moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[bknight] & mask) {addBitKnightMoves  (moves, board, tomove, mask, i, true, enemy, friendly);}   
			else if (board.piece[bqueen] & mask)  {addBitQueenMoves   (moves, board, tomove, mask, i, true, enemy, friendly);}
			else if (board.piece[bking] & mask)   {addBitKingMoves    (moves, board, tomove, mask, i, true, enemy, friendly);} 
			mask = mask << 1;
		}
		
		if (!onlyCaptures) {
			mask = 1;

			for (int i = 0; i < 64; i++){
				if (board.piece[bpawn] & mask)        {addBitPawnMoveBlack(moves, board, tomove, mask, i, false, enemy, friendly);}
				else if (board.piece[brook] & mask)   {addBitRookMoves    (moves, board, tomove, mask, i, false, enemy, friendly);} 
				else if (board.piece[bbishop] & mask) {addBitBishopMoves  (moves, board, tomove, mask, i, false, enemy, friendly);} 
				else if (board.piece[bknight] & mask) {addBitKnightMoves  (moves, board, tomove, mask, i, false, enemy, friendly);}   
				else if (board.piece[bqueen] & mask)  {addBitQueenMoves   (moves, board, tomove, mask, i, false, enemy, friendly);}
				else if (board.piece[bking] & mask)   {addBitKingMoves    (moves, board, tomove, mask, i, false, enemy, friendly);} 
				mask = mask << 1;
			}
		}
	}
	else {
		/* if we search the moves for white, start with the moves close to the
		 * black backrank.*/
		u64 mask = 1LLU << (7 + 7 * nort);

		for (int i = 63; i >= 0; i--){
			if (board.piece[wpawn] & mask)        {addBitPawnMoveWhite(moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[wrook] & mask)   {addBitRookMoves    (moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[wbishop] & mask) {addBitBishopMoves  (moves, board, tomove, mask, i, true, enemy, friendly);} 
			else if (board.piece[wknight] & mask) {addBitKnightMoves  (moves, board, tomove, mask, i, true, enemy, friendly);}  
			else if (board.piece[wqueen] & mask)  {addBitQueenMoves   (moves, board, tomove, mask, i, true, enemy, friendly);}
			else if (board.piece[wking] & mask)   {addBitKingMoves    (moves, board, tomove, mask, i, true, enemy, friendly);} 
			mask = mask >> 1;
		}
		
		if (!onlyCaptures){
			mask = 1LLU << (7 + 7 * nort);

			for (int i = 63; i >= 0; i--){
				if (board.piece[wpawn] & mask)        {addBitPawnMoveWhite(moves, board, tomove, mask, i, false, enemy, friendly);}
				else if (board.piece[wrook] & mask)   {addBitRookMoves    (moves, board, tomove, mask, i, false, enemy, friendly);} 
				else if (board.piece[wbishop] & mask) {addBitBishopMoves  (moves, board, tomove, mask, i, false, enemy, friendly);} 
				else if (board.piece[wknight] & mask) {addBitKnightMoves  (moves, board, tomove, mask, i, false, enemy, friendly);}  
				else if (board.piece[wqueen] & mask)  {addBitQueenMoves   (moves, board, tomove, mask, i, false, enemy, friendly);}
				else if (board.piece[wking] & mask)   {addBitKingMoves    (moves, board, tomove, mask, i, false, enemy, friendly);} 
				mask = mask >> 1;
			}
		}
	}
	
	/* after these loops are done, we should iterate over the new legal 
	 * moves, and generate a hash for each */
	for (int i = 0; i < moves->size; i++){
		moves->boards[i].hashValue = hashPosition(&(moves->boards[i]), tomove);
	}
}

resultconst gameend(bitboard board, bool tomove){
	movearray moves;
	
	bitGenerateLegalmoves(&moves, board, tomove, true);
	if (moves.size != 0) return ongoing;
	bitGenerateLegalmoves(&moves, board, tomove, false);
	if (moves.size != 0) return ongoing;
	
	if (bitInCheck(&board, tomove)) return (tomove == white ? blackwon : whitewon);
	return draw;
}
