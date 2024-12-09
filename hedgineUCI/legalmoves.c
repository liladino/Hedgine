#include "legalmoves.h"

#define WKINGSIDE 1
#define WQUEENSIDE 2
#define BKINGSIDE 4
#define BQUEENSIDE 8

/* PAWN MOVE MASKS
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
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 00000000 = 0x00FFFFFFFFFFFF00LL
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
 * */
#define PAWN_TAKE_EAST    0x007F7F7F7F7F7F00LL
#define PAWN_TAKE_WEST    0x00FEFEFEFEFEFE00LL
#define PAWN_MOVE_FORWARD 0x00FFFFFFFFFFFF00LL
#define WPAWN_PROMOTE     0x00FF000000000000LL
#define WPAWN_HOME        0x00FFFFFFFFFFFF00LL
#define BPAWN_PROMOTE     0x00FFFFFFFFFFFF00LL
#define BPAWN_HOME        0x00FF000000000000LL

static inline u64 enemypieces(bitboard board, bool enemy){
	if (enemy == black){ //black
		return (board.piece[bking] | board.piece[bqueen] | board.piece[bbishop] | board.piece[bknight] | board.piece[bpawn] | board.piece[brook]); 
	}
	return (board.piece[wking] | board.piece[wqueen] | board.piece[wbishop] | board.piece[wknight] | board.piece[wpawn] | board.piece[wrook]); 
}


bool bitInCheck(bitboard board, bool tomove){
	u64 enemy = enemypieces(board, !tomove), friendly = enemypieces(board, tomove);
	int coloffset = tomove == white ? bking : wking;
	u64 piecemask = 1;
	
	int rank = -1, file = -1;
	for (int i = 0; i < 64; i++){
		if (board.piece[bking - coloffset] & piecemask){
			rank = i / 8; 
			file = i % 8;
			break;
		}
		piecemask = piecemask << 1;
	}
	if (rank == -1 || file == -1) return false;
	
	//pawn
	u64 searchmask = piecemask;
	if (tomove == white){
		if ((file != 0 && rank < 6) && ((searchmask << noWe) & board.piece[bpawn])) return true;
		if ((file != 7 && rank < 6) && ((searchmask << noEa) & board.piece[bpawn])) return true;
	}
	else{
		if ((file != 0 && rank > 1) && ((searchmask >> soWe) & board.piece[wpawn])) return true;
		if ((file != 7 && rank > 1) && ((searchmask >> soEa) & board.piece[wpawn])) return true;
	}
	
	//rook & queen dirs
	for (int i = file; i > 0; i--){
		searchmask = searchmask >> west;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask;
	for (int i = file; i < 7; i++){
		searchmask = searchmask << east;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask;
	for (int i = rank; i > 0; i--){
		searchmask = searchmask >> sout;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask;
	for (int i = rank; i < 7; i++){
		searchmask = searchmask << nort;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask;
	
	//bishop & queen dirs
	int i = rank, j = file;
	while (i > 0 && j > 0){
		searchmask = searchmask >> soWe; i--; j--;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask; i = rank, j = file;
	while (i > 0 && j < 7){
		searchmask = searchmask >> soEa;  i--; j++;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask; i = rank, j = file;
	while (i < 7 && j < 7){
		searchmask = searchmask << noEa; i++; j++;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			break;
		}
	}
	searchmask = piecemask; i = rank, j = file;
	while (i < 7 && j > 0){
		searchmask = searchmask << noWe; i++; j--;
		if (searchmask & friendly) break;
		if ((searchmask & enemy)){
			if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			break;
		}
	}
	
	searchmask = piecemask;
	//horsey
	if (rank < 7 && file < 6 && (searchmask << (east * 2 + nort) & board.piece[wknight + coloffset])) return true;
	if (rank < 7 && file > 1 && (searchmask << (nort - west * 2) & board.piece[wknight + coloffset])) return true;
	if (rank < 6 && file < 7 && (searchmask << (east + nort * 2) & board.piece[wknight + coloffset])) return true;
	if (rank < 6 && file > 0 && (searchmask << (nort * 2 - west) & board.piece[wknight + coloffset])) return true;
	
	if (rank > 0 && file < 6 && (searchmask >> (sout - west * 2) & board.piece[wknight + coloffset])) return true;
	if (rank > 0 && file > 1 && (searchmask >> (sout + east * 2) & board.piece[wknight + coloffset])) return true;
	if (rank > 1 && file < 7 && (searchmask >> (sout * 2 - west) & board.piece[wknight + coloffset])) return true;
	if (rank > 1 && file > 0 && (searchmask >> (sout * 2 + east) & board.piece[wknight + coloffset])) return true;
	
	//king
	if (rank < 7 && file > 0 && (searchmask << (noWe) & board.piece[wking + coloffset])) return true;
	if (rank < 7 && (searchmask << (nort) & board.piece[wking + coloffset])) return true;
	if (rank < 7 && file < 7 && (searchmask << (noEa) & board.piece[wking + coloffset])) return true;
	
	if (file < 7 && (searchmask << (east) & board.piece[wking + coloffset])) return true;
	if (file > 0 && (searchmask >> (west) & board.piece[wking + coloffset])) return true;
	
	if (rank > 0 && file > 0 && (searchmask >> (soWe) & board.piece[wking + coloffset])) return true;
	if (rank > 0 && (searchmask >> (sout) & board.piece[wking + coloffset])) return true;
	if (rank > 0 && file < 7 && (searchmask >> (soEa) & board.piece[wking + coloffset])) return true;
	
	return false;
}

static inline void deletePiece(bitboard *board, const u64 movemask, const int start, const int end, const int rank, const int file){
	if (board->piece[wrook] & movemask){
		if ((1LL << 7) & movemask) board->castlerights &= ~WKINGSIDE;
		else {if (1LL & movemask) board->castlerights &= ~WQUEENSIDE;}
	}
	else if (board->piece[brook] & movemask){
		if ((1LL << (7 + nort * 7)) & movemask) board->castlerights &= ~BKINGSIDE;
		else {if ((1LL << (nort * 7)) & movemask) board->castlerights &= ~BQUEENSIDE;}
	}
	
	u64 inverse = (~movemask);
	board->piece[0] &= inverse;
	board->piece[1] &= inverse;
	board->piece[2] &= inverse;
	board->piece[3] &= inverse;
	board->piece[4] &= inverse;
	board->piece[5] &= inverse;
	board->piece[6] &= inverse;
	board->piece[7] &= inverse;
	board->piece[8] &= inverse;
	board->piece[9] &= inverse;
	board->piece[10] &= inverse;
	board->piece[11] &= inverse;
}

static inline void setcastlingrights(bitboard *board, bool tomove, const int rank, const int file){
	//if we moved a rook from the corner, delete the adequate castling rights for that side.
	if (tomove == white && rank == 0){
		if (file == 0){
			hashCastleO(board, 1);
			board->castlerights &= ~(WQUEENSIDE);
		}
		if (file == 7){
			hashCastleO(board, 0);
			board->castlerights &= ~(WKINGSIDE);
		}
	}
	if (tomove == black && rank == 7){
		if (file == 0){
			hashCastleO(board, 3);
			board->castlerights &= ~(BQUEENSIDE);
		}
		if (file == 7){
			hashCastleO(board, 2);
			board->castlerights &= ~(BKINGSIDE);
		}
	}
}

static void orthogonalDir(piecenums RorQ, bitboard *legalmoves, int *array_index, bitboard board, bool tomove, bool , u64 enemy, u64 friendly, u64 piecemask, int sq){
	bitboard copy = board;
	int coloffset = tomove == black ? bking : 0; 
	u64 withoutpiece = board.piece[RorQ + coloffset];
	withoutpiece = withoutpiece & (~piecemask); //deleting the current bit, where the piece stands
			
	const int rank = sq / 8; 
	const int file = sq % 8;
	u64 movemask = piecemask;
	
	int start = (tomove==black ? 0 : bking); //reverse offset for iterating throu enemy pieces
	
	for (int j = file; j > 0; j--){
		movemask = movemask >> west;
		if (movemask & friendly) //friendly piece in the way
			break;
		if (movemask & enemy){ //enemy piece in the way
			//delete the enemy piece
			if (!onlyCaptures) break;
			deletePiece(&board, movemask, start, start + bking, rank, j-1);
			
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, rank, file);
			//move the piece to the square
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, rank*8+j-1, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, rank*8+j-1, RorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, rank, j);
			
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			//add move to movearray
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, rank*8+j-1, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, rank*8+j-1, RorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
			
	for (int j = file; j < 7; j++){
		movemask = movemask << east;
		if (movemask & friendly) //friendly piece in the way
			break;
		if (movemask & enemy){ //enemy piece in the way
			if (!onlyCaptures) break;
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, rank, j);
			deletePiece(&board, movemask, start, start + bking, rank, j+1);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, rank*8+j+1, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, rank*8+j+1, RorQ + coloffset);
			}
			
			break;
		}
		if (!onlyCaptures){
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, rank, j);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, rank*8+j+1, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, rank*8+j+1, RorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
			
	for (int j = rank; j > 0; j--){
		movemask = movemask >> sout;
		if (movemask & friendly) //friendly piece in the way
			break;
		if (movemask & enemy){ //enemy piece in the way
			if (!onlyCaptures) break;
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, j, file);
			deletePiece(&board, movemask, start, start + bking, j-1, file);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, (j-1)*8+file, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, (j-1)*8+file, RorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, j, file);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
				
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, (j-1)*8+ file, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, (j-1)*8+ file, RorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
			
	for (int j = rank; j < 7; j++){
		movemask = movemask << nort;
		if (movemask & friendly) //friendly piece in the way
			break;
		if (movemask & enemy){ //enemy piece in the way
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, j, file);
			if (!onlyCaptures) break;
			deletePiece(&board, movemask, start, start + bking, j+1, file);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, (j+1)*8+ file, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, (j+1)*8+ file, RorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			if (RorQ == wrook && board.castlerights) setcastlingrights(&board, tomove, j, file);
			board.piece[RorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
					
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, (j+1)*8+ file, RorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, (j+1)*8+ file, RorQ + coloffset);
			}
		}
	}
}

static void diagonalDir(piecenums BorQ, bitboard *legalmoves, int *array_index, bitboard board, bool tomove, bool onlyCaptures, u64 enemy, u64 friendly, u64 piecemask, int sq){
	bitboard copy = board;
	int coloffset = tomove == black ? bking : 0; 
	u64 withoutpiece = board.piece[BorQ + coloffset];
	withoutpiece = withoutpiece & (~piecemask); //deleting the current bit, where the piece stands
			
	const int rank = sq / 8; 
	const int file = sq % 8;
	u64 movemask = piecemask;
				
	int start = (tomove==black ? 0 : bking);
	int i = rank, j = file;
	while (i > 0 && j > 0){
		movemask = movemask >> soWe; i--; j--;
		if (movemask & friendly) break;  //friendly piece in the way
		if (movemask & enemy){ //enemy piece in the way
			//delete the enemy piece
			if (!onlyCaptures) break;

			deletePiece(&board, movemask, start, start + bking, i, j);
			//move the piece to the square
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
				
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			//add move to movearray
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
	i = rank, j = file;
	while (i > 0 && j < 7){
		movemask = movemask >> soEa;  i--; j++;
		if (movemask & friendly) break;  //friendly piece in the way
		if (movemask & enemy){ //enemy piece in the way
			if (!onlyCaptures) break;
			deletePiece(&board, movemask, start, start + bking, i, j);
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
	i = rank, j = file;
	while (i < 7 && j < 7){
		movemask = movemask << noEa; i++; j++;
		if (movemask & friendly) break;  //friendly piece in the way
		if (movemask & enemy){ //enemy piece in the way
			if (!onlyCaptures) break;
			deletePiece(&board, movemask, start, start + bking, i, j);
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
		}
	}
	board = copy;
	movemask = piecemask;
	i = rank, j = file;
	while (i < 7 && j > 0){
		movemask = movemask << noWe; i++; j--;
		if (movemask & friendly) break;  //friendly piece in the way
		if (movemask & enemy){ //enemy piece in the way
			if (!onlyCaptures) break;
			deletePiece(&board, movemask, start, start + bking, i, j);
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
			break;
		}
		if (!onlyCaptures){
			board.piece[BorQ + coloffset] = withoutpiece | movemask; 
			//~ (*attackedsquares) |= movemask;
			
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
				legalmoves[*array_index] = board;
				(*array_index)++;
				hashPieceIO(&board, i * 8 + j, BorQ + coloffset);
			}
		}
	}
}

static void addBitSlidingMoves(bitboard *legalmoves, int *array_index, bitboard board, bool tomove, bool onlyCaptures, u64 enemy, u64 friendly){
	hashTomoveIO(&board);
	hashEnPassantIO(&board, 8);
	board.enpassanttarget = 0;
	//char board2d[12][12];
	u64 piecemask = 1;
	int coloffset = tomove == black ? bking : 0; 
	for (int i = 0; i < 64; i++){
		if (board.piece[wrook + coloffset] & piecemask){
			hashPieceIO(&board, i, wrook + coloffset); //hash the current piece out
			orthogonalDir(wrook, legalmoves, array_index,board, tomove, onlyCaptures, enemy, friendly, piecemask, i);
			hashPieceIO(&board, i, wrook + coloffset); //hash back in
		}
		else if (board.piece[wqueen + coloffset] & piecemask){
			hashPieceIO(&board, i, wqueen + coloffset);
			orthogonalDir(wqueen, legalmoves, array_index,board, tomove, onlyCaptures, enemy, friendly, piecemask, i);
			diagonalDir(wqueen, legalmoves, array_index,board, tomove, onlyCaptures, enemy, friendly, piecemask, i);
			hashPieceIO(&board, i, wqueen + coloffset);
		}
		else if (board.piece[wbishop + coloffset] & piecemask){
			hashPieceIO(&board, i, wbishop + coloffset);
			diagonalDir(wbishop, legalmoves, array_index,board, tomove, onlyCaptures, enemy, friendly, piecemask, i);
			hashPieceIO(&board, i, wbishop + coloffset);
		}
		
		piecemask = piecemask << 1;
	}
}


static void addBitKnightMoves(bitboard *legalmoves, int *array_index, bitboard board, bool tomove, bool onlyCaptures, u64 enemy, u64 friendly){	
	hashTomoveIO(&board);
	hashEnPassantIO(&board, 8);
	board.enpassanttarget = 0;
	bitboard copy = board;
	u64 piecemask = 1;
	int coloffset = tomove == black ? bking : 0; 
	int start = (tomove==black ? 0 : bking);
	if (board.piece[wknight + coloffset] == 0) return;
	
	for (int i = 0; i < 64; i++){
		if (board.piece[wknight + coloffset] & piecemask){
			hashPieceIO(&board, i, wknight + coloffset);
			hashPieceIO(&copy, i, wknight + coloffset); //hash piece out from the copy too, so it wont be lost later
			const int rank = i / 8; 
			const int file = i % 8;
			u64 withoutpiece = board.piece[wknight + coloffset];
			withoutpiece = withoutpiece & (~piecemask); 
			
			u64 movemask = piecemask;
			if (rank < 7 && file < 6){
				movemask = movemask << (east * 2 + nort);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank + 1, file + 2);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*east + nort, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*east + nort, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank < 7 && file > 1 ){
				movemask = movemask << (nort - west * 2);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank + 1, file - 2);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + nort - 2*west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + nort - 2*west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank < 6 && file < 7){
				movemask = movemask << (east + nort * 2);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank + 2, file + 1);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*nort + east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*nort + east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank < 6 && file > 0){
				movemask = movemask << (nort * 2 - west);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank + 2, file - 1);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*nort - west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i + 2*nort - west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank > 0 && file < 6){
				movemask = movemask >> (sout - east * 2);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank - 1, file + 2);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - sout + 2*east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - sout + 2*east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank > 0 && file > 1){
				movemask = movemask >> (sout + west * 2);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank - 1, file - 2);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - sout - 2*west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - sout - 2*west, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask; board = copy;
			}
			if (rank > 1 && file < 7){
				movemask = movemask >> (sout * 2 - east);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank - 2, file + 1);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - 2*sout + east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - 2*sout + east, wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
					movemask = piecemask; board = copy;
			}
			if (rank > 1 && file > 0){
				movemask = movemask >> (sout * 2 + west);
				if ((movemask & enemy) && onlyCaptures){
					deletePiece(&board, movemask, start, start + bking, rank - 2, file - 1);
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - (sout * 2 + west), wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				if (!onlyCaptures && (movemask & (friendly | enemy)) == 0){
					board.piece[wknight + coloffset] = withoutpiece | movemask; 
					//~ (*attackedsquares) |= movemask;
					if (!bitInCheck(board, tomove)) {
						hashPieceIO(&board, i - (sout * 2 + west), wknight + coloffset);
						legalmoves[*array_index] = board;
						(*array_index)++;
					}
				}
				movemask = piecemask;
			}
			hashPieceIO(&copy, i, wknight + coloffset); board = copy; //hash the piece back in
		}
		piecemask = piecemask << 1;
	}
}

static void addBitKingMoves(bitboard *legalmoves, int *array_index, bitboard board, bool tomove, bool onlyCaptures, u64 enemy, u64 friendly){
	hashTomoveIO(&board);
	hashEnPassantIO(&board, 8);
	board.enpassanttarget = 0;
	bitboard copy = board;
	u64 piecemask = 1;
	int coloffset = tomove == black ? bking : 0; 
	//coloffset += wking; 
	int start = (tomove==black ? 0 : bking);
	
	int i;
	for (i = 0; i < 64; i++){
		if (board.piece[wking + coloffset] & piecemask) break;
		piecemask = piecemask << 1;
	}
	hashPieceIO(&board, i, wking + coloffset);
	hashPieceIO(&copy, i, wking + coloffset);
	const int rank = i / 8; 
	const int file = i % 8;
	u64 withoutpiece = board.piece[wking + coloffset];
	withoutpiece = withoutpiece & (~piecemask); 
	
	if (rank < 7 && file > 0 && ((piecemask << noWe) & friendly) == 0){
		if (onlyCaptures && ((piecemask << noWe) & enemy)){
			deletePiece(&board, (piecemask << noWe), start, start + bking, rank + 1, file - 1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask << noWe); 
			//~ (*attackedsquares) |= (piecemask << noWe);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + noWe, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask << noWe) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask << noWe); 
			//~ (*attackedsquares) |= (piecemask << noWe);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + noWe, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	if (rank < 7 && ((piecemask << nort) & friendly) == 0){
		if (onlyCaptures && ((piecemask << nort) & enemy)){
			deletePiece(&board, (piecemask << nort), start, start + bking, rank + 1, file);
			board.piece[wking + coloffset] = withoutpiece | (piecemask << nort); 
			//~ (*attackedsquares) |= (piecemask << nort);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + nort, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask << nort) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask << nort); 
			//~ (*attackedsquares) |= (piecemask << nort);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + nort, wking + coloffset);hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	if (rank < 7 && file < 7 && ((piecemask << noEa) & friendly) == 0){
		if (onlyCaptures && ((piecemask << noEa) & enemy)){
			deletePiece(&board, (piecemask << noEa), start, start + bking, rank + 1, file + 1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask << noEa); 
			//~ (*attackedsquares) |= (piecemask << noEa);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + noEa, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask << noEa) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask << noEa); 
			//~ (*attackedsquares) |= (piecemask << noEa);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + noEa, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	
	if (rank > 0 && file > 0 && ((piecemask >> soWe) & friendly) == 0){
		if (onlyCaptures && ((piecemask >> soWe) & enemy)){
			deletePiece(&board, (piecemask >> soWe), start, start + bking, rank - 1, file -1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> soWe); 
			//~ (*attackedsquares) |= (piecemask >> soWe);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - soWe, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask >> soWe) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> soWe); 
			//~ (*attackedsquares) |= (piecemask >> soWe);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - soWe, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	
	if (rank > 0 && ((piecemask >> sout) & friendly) == 0){
		if (onlyCaptures && ((piecemask >> sout) & enemy)){
			deletePiece(&board, (piecemask >> sout), start, start + bking, rank - 1, file);
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> sout); 
			//~ (*attackedsquares) |= (piecemask >> sout);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - sout, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask >> sout) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> sout); 
			//~ (*attackedsquares) |= (piecemask >> sout);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - sout, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	if (rank > 0 && file < 7 && ((piecemask >> soEa) & friendly) == 0){
		if (onlyCaptures && ((piecemask >> soEa) & enemy)){
			deletePiece(&board, (piecemask >> soEa), start, start + bking, rank - 1, file + 1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> soEa); 
			//~ (*attackedsquares) |= (piecemask >> soEa);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - soEa, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask >> soEa) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> soEa); 
			//~ (*attackedsquares) |= (piecemask >> soEa);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - soEa, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
	}
	
	if (file < 7 && ((piecemask << east) & friendly) == 0){
		if (onlyCaptures && ((piecemask << east) & enemy)){
			deletePiece(&board, (piecemask << east), start, start + bking, rank, file + 1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask << east); 
			//~ (*attackedsquares) |= (piecemask << east);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + east, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask << east) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask << east); 
			//~ (*attackedsquares) |= (piecemask << east);
			bool castleallow = false;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i + east, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
				castleallow = true;
			}
			board = copy;
			
			if (castleallow && !bitInCheck(board, tomove) && (board.castlerights & (WKINGSIDE << (tomove == white ? 0 : 2))) && file == 4 && rank == (tomove == white ? 0 : 7) && ((piecemask << (2 * east)) & (friendly | enemy)) == 0){
				board.piece[wking + coloffset] = withoutpiece | (piecemask << (2 * east)); 
				u64 rookmask = 1;
				if (tomove == white) rookmask = rookmask << 7;
				else rookmask = rookmask << (7 + nort * 7);
				board.piece[wrook + coloffset] &= (~rookmask); //delete rook from h file
				board.piece[wrook + coloffset] |= (rookmask >> 2); //f file rook
				if (!bitInCheck(board, tomove)) {
					hashPieceIO(&board, i + 2*east, wking + coloffset);
					hashPieceIO(&board, i + 3*east, wrook + coloffset); //deleting the rook
					hashPieceIO(&board, i + 1*east, wrook + coloffset); //adding back the rook
					hashCastleO(&board, tomove == white ? 0 : 2);
					hashCastleO(&board, tomove == white ? 1 : 3);
					board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
					legalmoves[*array_index] = board;
					(*array_index)++;
				}
			}
			board = copy;
		}
	}
	if (file > 0 && ((piecemask >> west) & friendly) == 0){
		if (onlyCaptures && ((piecemask >> west) & enemy)){
			deletePiece(&board, (piecemask >> west), start, start + bking, rank, file - 1);
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> west); 
			//~ (*attackedsquares) |= (piecemask >> west);
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - west, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board = copy;
		}
		if (!onlyCaptures && ((piecemask >> west) & enemy) == 0){
			board.piece[wking + coloffset] = withoutpiece | (piecemask >> west); 
			//~ (*attackedsquares) |= (piecemask >> west);
			bool castleallow = false;
			if (!bitInCheck(board, tomove)) {
				hashPieceIO(&board, i - west, wking + coloffset); hashCastleO(&board, tomove == white ? 0 : 2); hashCastleO(&board, tomove == white ? 1 : 3);
				board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
				legalmoves[*array_index] = board;
				(*array_index)++;
				castleallow = true;
			}
			board = copy;
			
			if (castleallow && !bitInCheck(board, tomove) && (board.castlerights & (WQUEENSIDE << (tomove == white ? 0 : 2))) && file == 4 && rank == (tomove == white ? 0 : 7) && ((piecemask >> (2 * west)) & (friendly | enemy)) == 0 && ((piecemask >> (3 * west)) & (friendly | enemy)) == 0){
				board.piece[wking + coloffset] = withoutpiece | (piecemask >> (2 * west)); 
				u64 rookmask = 1;
				if (tomove == black) rookmask = rookmask << (nort * 7);
				board.piece[wrook + coloffset] &= (~rookmask); //delete rook from a file
				board.piece[wrook + coloffset] |= (rookmask << 3); //d file rook
				if (!bitInCheck(board, tomove)) {
					hashPieceIO(&board, i - 2*west, wking + coloffset);
					hashPieceIO(&board, i - 4*west, wrook + coloffset); //deleting the rook
					hashPieceIO(&board, i - 1*west, wrook + coloffset); //adding back the rook
					hashCastleO(&board, tomove == white ? 0 : 2);
					hashCastleO(&board, tomove == white ? 1 : 3);
					board.castlerights &= (~ (3 << (tomove == white ? 0 : 2)));
					legalmoves[*array_index] = board;
					(*array_index)++;
				}
			}
		}
	}
}

static void addBitWPawnMove(bitboard *legalmoves, bitboard board, bool tomove, u64 piece, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = legalmoves->size;
	bitboard copy = board;
	
	const int rank = i / 8; 
	const int file = i % 8;
	u64 withoutpiece = board.piece[wpawn + coloffset];
	withoutpiece = withoutpiece & (~piece); 
	
	u64 moveforward = piece << 8, takeleft = piece << noWe, takeright = piece << noEa;
	
	if (!onlyCaptures && piece & PAWN_MOVE_FORWARD){
		//moving forward 1 square
		board.piece[bpawn] = withoutpiece | moveforward; 
		board.enpassanttarget = 0;
		if (!bitInCheck(board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
		
		if (piece & WPAWN_HOME){
			
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	u64 movemask = piece;
	if (rank > 1 && ((movemask >> sout) & (enemy | friendly)) == 0 && !onlyCaptures){
		board.piece[bpawn] = withoutpiece | (movemask >> sout); 
		board.enpassanttarget = 0;
		if (!bitInCheck(board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
		if (rank == 6 && ((movemask >> (sout * 2)) & (enemy | friendly)) == 0 && !onlyCaptures){
			board.piece[bpawn] = copy.piece[bpawn]; //reset pawn to where it was
			board.piece[bpawn] = withoutpiece | (movemask >> (2 * sout)); 
			if (!bitInCheck(board, tomove)) {
				board.enpassanttarget = movemask >> sout;
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
		}
	}
	board = copy;
	if (rank == 1 && ((movemask >> sout) & (enemy | friendly)) == 0 && onlyCaptures){ //technically promotion isnt taking, but its good to consider it among the first ones
		board.piece[bpawn] = withoutpiece;
		board.enpassanttarget = 0;
		for (int k = bqueen; k < bpawn; k++){
			u64 temp = board.piece[k];
			board.piece[k] |= (movemask >> sout); 
			if (!bitInCheck(board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			board.piece[k] = temp;
		}
	}
	board = copy;
	if (rank > 1 && file > 0 && onlyCaptures){ //takes west
		//~ (*attackedsquares) |= (movemask >> soWe); 
		if ((movemask >> soWe) & enemy){
			board.piece[bpawn] = withoutpiece | (movemask >> soWe); 
			board.enpassanttarget = 0;
			deletePiece(&board, movemask >> soWe, wqueen, bking, rank - 1, file - 1);
			if (!bitInCheck(board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
		}
	}
	board = copy;
	if (rank > 1 && file < 7 && onlyCaptures){ //takes east
		//~ (*attackedsquares) |= (movemask >> soEa); 
		if ((movemask >> soEa) & enemy){
			board.piece[bpawn] = withoutpiece | (movemask >> soEa); 
			board.enpassanttarget = 0;
			deletePiece(&board, movemask >> soEa, wqueen, bking, rank - 1, file + 1);
			if (!bitInCheck(board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
		}
	}
	board = copy;
	if (rank == 1 && file > 0 && onlyCaptures){ //takes west and promotes
		//~ (*attackedsquares) |= (movemask >> soWe); 
		if ((movemask >> soWe) & enemy){
			board.piece[bpawn] = withoutpiece; 
			board.enpassanttarget = 0;
			deletePiece(&board, movemask >> soWe, wqueen, bking, rank - 1, file -1);
			for (int k = bqueen; k < bpawn; k++){
				u64 temp = board.piece[k];
				board.piece[k] |= (movemask >> soWe); 
				if (!bitInCheck(board, tomove)) {
					legalmoves[*array_index] = board;
					(*array_index)++;
				}
				board.piece[k] = temp;
			}
		}
	}
	board = copy;
	if (rank == 1 && file < 7 && onlyCaptures){ //takes east and promotes
		//~ (*attackedsquares) |= (movemask >> soEa); 
		if ((movemask >> soEa) & enemy){
			board.piece[bpawn] = withoutpiece; 
			board.enpassanttarget = 0;
			deletePiece(&board, movemask >> soEa, wqueen, bking, rank - 1, file + 1);
			for (int k = bqueen; k < bpawn; k++){
				u64 temp = board.piece[k];
				board.piece[k] |= (movemask >> soEa); 
				if (!bitInCheck(board, tomove)) {
					legalmoves[*array_index] = board;
					(*array_index)++;
				}
				board.piece[k] = temp;
			}
		}
	}
	board = copy;
	if (rank == 3 && file > 0 && ((movemask >> soWe) & board.enpassanttarget) && onlyCaptures){ //en passant west
		board.piece[bpawn] = withoutpiece | (movemask >> soWe); 
		board.enpassanttarget = 0;
		deletePiece(&board, movemask >> west, wpawn, bking, rank, file - 1);
		if (!bitInCheck(board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
	}
	else if (rank == 3 && file < 7 && ((movemask >> soEa) & board.enpassanttarget) && onlyCaptures){ //en passant east, else cos two en passants arent possible
		board.piece[bpawn] = withoutpiece | (movemask >> soEa); 
		board.enpassanttarget = 0;
		deletePiece(&board, movemask << east, wpawn, bking, rank, file + 1);
		if (!bitInCheck(board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
	}
}


void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	bitboard* legalmoves = moves->boards;
	moves->size = 0;
	
	if (tomove == black){
		/*if we search the moves for black, start with the moves close to the
		 * white backrank.*/
		u64 mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask) {addBitBPawnMove(legalmoves, board, tomove, mask, i, true, enemy, friendly);} //add pawn move
			else if (board.piece[brook] & mask) {} 
			else if (board.piece[bbishop] & mask) {} 
			else if (board.piece[bknight] & mask) {}  
			else if (board.piece[bqueen] & mask) {}
			else if (board.piece[bking] & mask) {} 
			mask = mask << 1;
		}
		
		if (onlyCaptures) return;
		
		//look for captures
		mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask) {} //add pawn move
			else if (board.piece[brook] & mask) {} 
			else if (board.piece[bbishop] & mask) {} 
			else if (board.piece[bknight] & mask) {}  
			else if (board.piece[bqueen] & mask) {}
			else if (board.piece[bking] & mask) {} 
			mask = mask << 1;
		}
	}
	else {
		//look for white moves
		
	}
	
	addBitPawnMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	addBitKnightMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	addBitSlidingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	addBitKingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	
	if (onlyCaptures) return;
	
	addBitSlidingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	addBitKnightMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	addBitPawnMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);/**/
	addBitKingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
}

resultconst gameend(bitboard board, bool tomove){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	bitboard legalmoves[MAXMOVECOUNT_INPOS];
	int array_index = 0;
	
	addBitKingMoves(legalmoves, &array_index, board, tomove, true, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitKingMoves(legalmoves, &array_index, board, tomove, false, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitSlidingMoves(legalmoves, &array_index, board, tomove, true, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitSlidingMoves(legalmoves, &array_index, board, tomove, false, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitKnightMoves(legalmoves, &array_index, board, tomove, true, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitKnightMoves(legalmoves, &array_index, board, tomove, false, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitPawnMoves(legalmoves, &array_index, board, tomove, false, enemy, friendly);
	if (array_index != 0) return ongoing;
	addBitPawnMoves(legalmoves, &array_index, board, tomove, true, enemy, friendly);
	if (array_index != 0) return ongoing;
	
	if (bitInCheck(board, tomove)) return (tomove == white ? blackwon : whitewon);
	return draw;
}


/*
move converttomove(char oldboard[12][12], char newboard[12][12]){
	//from 2 boards generate a move
}
* 
* 
* */



