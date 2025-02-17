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

u64 whiteKingIndex;
u64 blackKingIndex;

#include "magicvalues.txt"


static inline u64 enemypieces(bitboard board, bool enemy){
	if (enemy == black){ //black
		return (board.piece[bking] | board.piece[bqueen] | board.piece[bbishop] | board.piece[bknight] | board.piece[bpawn] | board.piece[brook]); 
	}
	return (board.piece[wking] | board.piece[wqueen] | board.piece[wbishop] | board.piece[wknight] | board.piece[wpawn] | board.piece[wrook]); 
}


bool bitInCheck(const bitboard* const board, bool tomove){
	//~ int coloffset = tomove == white ? bking : wking;
	
	u64 piecemask = 1;
	if (tomove == white){
		piecemask = piecemask << whiteKingIndex;
		
		if (board->piece[bpawn] & wpawnTakes[whiteKingIndex]) return true;
		if (board->piece[bknight] & knightMoves[whiteKingIndex]) return true;
	}
	else{
		piecemask = piecemask << blackKingIndex;
		
		if (board->piece[wpawn] & bpawnTakes[blackKingIndex]) return true;
		if (board->piece[wknight] & knightMoves[blackKingIndex]) return true;
	}
		
	//~ u64 enemy = enemypieces(board, !tomove), friendly = enemypieces(board, tomove);
	
	/* TODO - LOOK FOR BISHOPS. QUEENS, KINGS HERE */
	
	//king
	if (board->piece[wking] & kingAttacks[blackKingIndex]){
		/* the king attacks are symmetrical, so it's enought to check one 
		 * side; if the orther attacks the first, the first attacks the other 
		 * too.*/ 
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

/* pops the least significant bit and returns an u64 with that bit only */
static inline u64 pop_lsb(u64 *var) {
	u64 lsb = *var & -*var;  // Isolate LS1B
	*var &= (*var - 1);      // Clear LS1B
	return lsb;
}

static void addBitKnightMoves(movearray* moves, bitboard board, bool tomove, u64 piece, int index, bool onlyCaptures, u64 enemy, u64 friendly){	
	bitboard copy = board;
	int coloffset = (tomove == white ? bking : wking);
	u64 withoutpiece = board.piece[wknight + coloffset] & (~piece);
	
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = knightMoves[index];
	
	while (possiblemoves) { 
		u64 currentmove = pop_lsb(&possiblemoves); 
		if (onlyCaptures && (enemy & currentmove)){
			board.piece[wknight + coloffset] = withoutpiece | currentmove; 
			board.enpassanttarget = 0;
			deletePiece(&board, currentmove);
			if (!bitInCheck(&board, tomove)) {
				legalmoves[(*array_index)++] = board;
			}
			board = copy;
		}
		else if (!onlyCaptures && 0 == (enemy & currentmove) && 0 == (friendly & currentmove)){
			board.piece[wknight + coloffset] = withoutpiece | currentmove; 
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
	u64 withoutpiece = board.piece[wpawn] & (~piece);
	int coloffset = tomove == white ? bking : wking;
	
	bitboard* legalmoves = moves->boards;
	int* array_index = &(moves->size);
	
	#define ADDKINGMOVE() do { board.enpassanttarget = 0; if (!bitInCheck(&board, tomove)) { legalmoves[(*array_index)++] = board; } board = copy; } while(0);
	
	if (onlyCaptures){
		/* these should be set when we trz to take east or west, so that the
		 * castling can know, if it will travel through a check */ 
		bool kingSideCastle = false, queenSideCastle = false;
		
		//set castle possibility shit
		if ((tomove == white && i == 4) || (tomove == black && i == 60)){
			if (((piece >> west) & (friendly | enemy)) == 0){
				//nothing is there, check if its in check, BUT DO NOT ADD THE MOVE
				board.piece[wking + coloffset] = piece >> west;
				board.enpassanttarget = 0; 
				if (!bitInCheck(&board, tomove)) { 
					queenSideCastle = true;
				} 
				board = copy;
			}
			if (((piece << east) & (friendly | enemy)) == 0){
				board.piece[wking + coloffset] = piece << east;
				board.enpassanttarget = 0; 
				if (!bitInCheck(&board, tomove)) { 
					kingSideCastle = true;
				} 
				board = copy;
			}			
		}
		
		//taking
		u64 possiblemoves = kingAttacks[i];
	
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			if (currentmove & enemy){
				board.piece[wking] = currentmove; //move the king bit
				board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
				ADDKINGMOVE();
			}
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
		u64 possiblemoves = kingAttacks[i];
	
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			if ((currentmove & (enemy | friendly)) == 0){
				board.piece[wking] = currentmove; //move the king bit
				board.castlerights &= (tomove == white ? 12 : 3); //delete castling rights
				ADDKINGMOVE();
			}
		}
	}
}

static void addBitPawnMoveWhite(movearray* moves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitboard* legalmoves = moves->boards;
	
	u64 withoutpiece = board.piece[wpawn] & (~piece);
	
	u64 moveforward = piece << 8, takewest = piece << noWe, takeeast = piece << noEa;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && piece & WPAWN_MOVE_FORWARD && !(moveforward & (enemy | friendly))){
		//moving forward 1 sqyare
		board.piece[wpawn] = withoutpiece | moveforward; 
		board.enpassanttarget = 0;
		if (!bitInCheck(&board, tomove)) {
			legalmoves[(*array_index)++] = board;
			
			if (piece & WPAWN_HOME){
				//moving 2 squares
				board.piece[wpawn] = withoutpiece | (piece << 16); 
				if ( ((PAWN_TAKE_EAST & piece) && ((piece >> west) & board.piece[bpawn]))
				  || ((PAWN_TAKE_WEST & piece) && ((piece << east) & board.piece[bpawn])) ){
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
	}
	else if (!onlyCaptures){
		return;
	}
	
	bitboard copy = board;
		
	if (onlyCaptures){
		/* Takes */
		u64 possiblecaptures = wpawnTakes[i];
	
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piece & WPAWN_MOVE_FORWARD)){
				/* Takes normally */
				board.piece[wpawn] = (withoutpiece | currentmove); //move the pawn
				deletePiece(&board, currentmove);
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
			else if ((currentmove & enemy) && (piece & WPAWN_PROMOTE) {
				/* Promotes AND takes */
				deletePiece(&temp, currentmove);
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
					
					board.piece[i];
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
				
				board.piece[i];
			}
			board = copy;
		}
	}
}


void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	bitboard* legalmoves = moves->boards;
	moves->size = 0;
	
	/* find the kings and store them in the whiteKingMask and blackKingMask 
	 * global variables, for the check detection function to effectively use 
	 * it. */
	u64 wKingMask = 1, bKingMask = 1;
	for (int i = 0; i < 64; i++){
		if (board.piece[wking] & wKingMask){
			whiteKingIndex = i;
		}
		else if (board.piece[bking] & bKingMask){
			blackKingIndex = i;
		}
		wKingMask = wKingMask << 1;
		bKingMask = bKingMask << 1;
	}
	
	if (tomove == black){
		/* if we search the moves for black, start with the moves close to the
		 * white backrank.*/
		u64 mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask)        {} 
			else if (board.piece[brook] & mask)   {} 
			else if (board.piece[bbishop] & mask) {} 
			else if (board.piece[bknight] & mask) {addBitKnightMoves(moves, board, tomove, mask, i, true, enemy, friendly);}   
			else if (board.piece[bqueen] & mask)  {}
			else if (board.piece[bking] & mask)   {addBitKingMoves(moves, board, tomove, mask, i, true, enemy, friendly);} 
			mask = mask << 1;
		}
		
		if (!onlyCaptures) {
			mask = 1;

			for (int i = 0; i < 64; i++){
				if (board.piece[bpawn] & mask)        {}
				else if (board.piece[brook] & mask)   {} 
				else if (board.piece[bbishop] & mask) {} 
				else if (board.piece[bknight] & mask) {addBitKnightMoves(moves, board, tomove, mask, i, false, enemy, friendly);}   
				else if (board.piece[bqueen] & mask)  {}
				else if (board.piece[bking] & mask)   {addBitKingMoves(moves, board, tomove, mask, i, false, enemy, friendly);} 
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
			else if (board.piece[wrook] & mask)   {} 
			else if (board.piece[wbishop] & mask) {} 
			else if (board.piece[wknight] & mask) {addBitKnightMoves(moves, board, tomove, mask, i, true, enemy, friendly);}  
			else if (board.piece[wqueen] & mask)  {}
			else if (board.piece[wking] & mask)   {addBitKingMoves(moves, board, tomove, mask, i, true, enemy, friendly);} 
			mask = mask >> 1;
		}
		
		if (!onlyCaptures){
			//look for captures
			mask = 1LLU << (7 + 7 * nort);

			for (int i = 0; i < 64; i++){
				if (board.piece[wpawn] & mask)        {addBitPawnMoveWhite(moves, board, tomove, mask, i, false, enemy, friendly);}
				else if (board.piece[wrook] & mask)   {} 
				else if (board.piece[wbishop] & mask) {} 
				else if (board.piece[wknight] & mask) {addBitKnightMoves(moves, board, tomove, mask, i, false, enemy, friendly);}  
				else if (board.piece[wqueen] & mask)  {}
				else if (board.piece[wking] & mask)   {addBitKingMoves(moves, board, tomove, mask, i, false, enemy, friendly);} 
				mask = mask >> 1;
			}
		}
	}
	/* after these loops are done, we should iterate over the new legal 
	 * moves, and generate a hash for each */
	generate_hash:
	for (int i = 0; i < moves->size; i++){
		moves->boards[i].hashValue = hashPosition(&(moves->boards[i]), tomove);
	}
	
	
	// OLD SHIT:
	//~ addBitPawnMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitKnightMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitSlidingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitKingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	
	//~ if (onlyCaptures) return;
	
	//~ addBitSlidingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	//~ addBitKnightMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	//~ addBitPawnMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);/**/
	//~ addBitKingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
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
