#include "headers/moves.h"

/*atalakit egy mezot mezoszamokka
 */
squarenums makesquarenum(square a){
	squarenums b;
	b.file = a.file - 'a' + 2;
	b.rank = a.rank + 1;
	return b;
}

squarenums makesquarenum2(int i){
	squarenums b;
	b.file = i % 8 + 2;
	b.rank = i / 8 + 2;
	return b;
}

/* initializes a move from the starting and ending square numbers and from the promotion charcter
 * */
move initializemove(squarenums from, squarenums to, int promotion){
	move m;
	m.from.file = from.file;
	m.from.rank = from.rank;
	m.to.file = to.file;
	m.to.rank = to.rank;
	m.promotion = promotion;
	return m;
}

/* sets a move from the starting and ending squares and from the promotion charcter
 * */
void setmove(move *m, square from, square to, int promotion){
	m->from = makesquarenum(from);
	m->to = makesquarenum(to);
	m->promotion = promotion;
}

/*******************************************************************************
 * BITS
 ******************************************************************************/

move convertBitMoveToMove(const bitMove m){
	int temp;
	switch (m.promotion) {
		case wqueen:
			temp = 'q'; break;
		case wrook:
			temp = 'r'; break;
		case wknight:
			temp = 'n'; break;
		case wbishop:
			temp = 'b'; break;
		case bqueen:
			temp = 'q'; break;
		case brook:
			temp = 'r'; break;
		case bknight:
			temp = 'n'; break;
		case bbishop:
			temp = 'b'; break;
		default: 
			temp = 0; break;
	}
	return initializemove(makesquarenum2(m.from), makesquarenum2(m.to), temp);
} 

bool compareBitMoveToMove(const bitMove bm, const move m){
	move curr = convertBitMoveToMove(bm);
	return (curr.from.file == m.from.file && curr.from.rank == m.from.rank && curr.to.file == m.to.file && curr.to.rank == m.to.rank && curr.promotion == m.promotion);
}
 
bitMove convertMoveToBitMove(const bitboard* const board, bool tomove, const move m) {
	movearray moves;
	bitGenerateLegalmoves(&moves, board, tomove, false);
	for (int i = 0; i < moves.size; i++){
		if (compareBitMoveToMove(moves.array[i], m)){
			return moves.array[i];
		}
	}
	return moves.array[0];
}
 
int isMoveInMoveArray(const movearray* legalmoves, const move m){
	for (int i = 0; i < legalmoves->size; i++){
		if (compareBitMoveToMove(legalmoves->array[i], m)){
			return i;
		}
	}
	return -1;
} 

int isMoveLegal(bitboard* board, const bool tomove, const move m){
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	int x = isMoveInMoveArray(&legalmoves, m);
	
	if (x == -1){
		return 1;
	}

	bitMove bm = legalmoves.array[x];
	bitUndo u = makeMove(board, bm);
	
	legalmoves.size = 0;
	bitGenerateLegalmoves(&legalmoves, board, !tomove, false);
	undoMove(board, bm, u);
	
	if (legalmoves.size == 0){
		return 2;
	}
	return 0;
}

bitboard boardConvert(char board2d[12][12], int castling[4], squarenums enpass, bool tomove){
	/*castling: [3:0] = qkQk
	* 0000 0101 => 
	* also 4 bit:
	* 				 0					1					0 					 1 (LSB) 
	* sotet vezeroldal, sotet kiralyoldal, vilagos vezeroldal, vilagos kiralyoldal
	 * */
	bitboard board = {0};
	
	u64 piecemask = 1;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board2d[i][j]){
				case 'P': board.piece[wpawn] |= piecemask; break;
				case 'p': board.piece[bpawn] |= piecemask; break;
				case 'B': board.piece[wbishop] |= piecemask;break;
				case 'b': board.piece[bbishop] |= piecemask; break;
				case 'N': board.piece[wknight] |= piecemask; break;
				case 'n': board.piece[bknight] |= piecemask; break;
				case 'R': board.piece[wrook] |= piecemask; break;
				case 'r': board.piece[brook] |= piecemask; break;
				case 'Q': board.piece[wqueen] |= piecemask; break;
				case 'q': board.piece[bqueen] |= piecemask; break;
				case 'K': board.piece[wking] |= piecemask; break;
				case 'k': board.piece[bking] |= piecemask; break;
			}
			
			piecemask = piecemask << 1;
		}
	}
	board.castlerights = 0;
	if (castling[3]) board.castlerights += 1; //q
	board.castlerights = board.castlerights << 1;
	if (castling[2]) board.castlerights += 1; //k
	board.castlerights = board.castlerights << 1;
	if (castling[1]) board.castlerights += 1; //Q
	board.castlerights = board.castlerights << 1;
	if (castling[0]) board.castlerights += 1; //K
	
	if (enpass.rank == -1) board.enpassanttarget = 0;
	else{
		int file = enpass.file - 2;
		int rank = enpass.rank - 2;
		board.enpassanttarget = 1LL << (rank * 8 + file);
	}
	board.hashValue = hashPosition(&board, tomove);
	return board;
}

void addPieceOnBoard(char board2d[12][12], u64 piece, char piecesymbol){
	u64 a = 1;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			if (piece & a) board2d[i][j] = piecesymbol;
			a = a << 1;
		}
	}
}

void boardConvertBack(char board2d[12][12], bitboard board){
	//reset
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (i >= 2 && i < 10 && j >= 2 && j < 10)
				board2d[i][j] = ' ';
			else
				board2d[i][j] = 0;
		}
	}
	addPieceOnBoard(board2d, board.piece[wpawn], 'P');
	addPieceOnBoard(board2d, board.piece[wknight], 'N');
	addPieceOnBoard(board2d, board.piece[wbishop], 'B');
	addPieceOnBoard(board2d, board.piece[wrook], 'R');
	addPieceOnBoard(board2d, board.piece[wqueen], 'Q');
	addPieceOnBoard(board2d, board.piece[wking], 'K');
	
	addPieceOnBoard(board2d, board.piece[bpawn], 'p');
	addPieceOnBoard(board2d, board.piece[bknight], 'n');
	addPieceOnBoard(board2d, board.piece[bbishop], 'b');
	addPieceOnBoard(board2d, board.piece[brook], 'r');
	addPieceOnBoard(board2d, board.piece[bqueen], 'q');
	addPieceOnBoard(board2d, board.piece[bking], 'k');
}


move boardConvertTomove(const bitboard* const board1, const bitboard* const board2, bool tomove){
	move m = {{-1, -1}, {-1, -1}, 0};
	int offset = (tomove == white ? 0 : bking);
	int differences = 0;
	for (int i = wking + offset; i < bking + offset; i++){
		if (board1->piece[i] != board2->piece[i]) {
			differences = 0; 
			u64 mask = 1;
			for (int j = 0; j < 64; j++){
				if ((board1->piece[i] & mask) != (board2->piece[i] & mask)){
					differences++;
					if ((board1->piece[i] & mask)) {
						m.from.rank = j / 8 + 2;
						m.from.file = j % 8 + 2;
					}
					else{
						m.to.rank = j / 8 + 2;
						m.to.file = j % 8 + 2;
					}
				}
				mask = mask << 1;
			}
			if (differences == 2){
				return m;//no need to check for castling, cos the king was searched firstly
			}
			else {
				if (i != wpawn + offset){
					//prom
					switch(i){
						case wqueen: m.promotion = 'q'; break;
						case bqueen: m.promotion = 'q'; break;
						case wrook: m.promotion = 'r'; break;
						case brook: m.promotion = 'r'; break;
						case wbishop: m.promotion = 'b'; break;
						case bbishop: m.promotion = 'b'; break;
						case wknight: m.promotion = 'n'; break;
						case bknight: m.promotion = 'n'; break;
					}
				}
				else {
					return m; //pawn is the last one to be checked
				}
			}
		}
	}
	return m;
}

bool lastMoveWasCapture(const bitboard* const lastboard, const move m, const bool tomove){
	u64 targetsquare = 1;
	targetsquare = targetsquare << (m.to.file - 2 + (m.to.rank - 2) * 8);
	
	if ( 
		(lastboard->piece[wpawn] & targetsquare)   | 
		(lastboard->piece[wknight] & targetsquare) | 
		(lastboard->piece[wbishop] & targetsquare) | 
		(lastboard->piece[wrook] & targetsquare)   | 
		(lastboard->piece[wqueen] & targetsquare)  |  
		(lastboard->piece[bpawn] & targetsquare)   | 
		(lastboard->piece[bknight] & targetsquare) | 
		(lastboard->piece[bbishop] & targetsquare) | 
		(lastboard->piece[brook] & targetsquare)   | 
		(lastboard->piece[bqueen] & targetsquare)
			){
		//if any piece stood on the square we move to, it was a capture.
		return true;
	}
	
	//en passant
	targetsquare = lastboard->enpassanttarget;
	if (targetsquare == 0) return false;
	
	if (tomove == white) {
		if (lastboard->piece[bpawn] & (targetsquare >> 8)) return true;
	}
	else {
		if (lastboard->piece[wpawn] & (targetsquare << 8)) return true;
	}
	
	return false;
}

int8_t mailbox[64] = 
	{-1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1};
	
void setMailBox(const bitboard* const board){
	u64 pieces = enemypieces(board, 1) | enemypieces(board, 0);
	u64 mask = 1; int i = 0;
	for (; i < 64; i++, mask <<= 1){
		if ((pieces & mask)){
			for (int j = 0; j < 12; j++){
				if ((board->piece[j] & mask)){ 
					mailbox[i] = j; 
				}
			}
		} 
		else {
			mailbox[i] = -1;
		}
	}
}

void printMailBox(){
	printf("Mailbox:\n");
	u64 mask = 1; int i = 0; bitboard b;
	memset(&b, 0, sizeof(b));
	
	for (; i < 64; i++, mask <<= 1){
		if (mailbox[i] != -1){
			b.piece[mailbox[i]] |= mask;
		}
	}
	
	printBitBoard2d(stdout, b);
}
	
static inline void setcastlingrights(bitboard* board, bitMove m){		
	if (board->castlerights == 0){
		return;
	}
	/* If something moved to the corner or something moved from the corner, 
	 * castling is no longer possible. */
	if (m.to == 0 || m.from == 0){
		hashCastleO(board, WQUEENSIDE);
		board->castlerights &= ~(WQUEENSIDE);
	}
	else if (m.to == 7 || m.from == 7){
		hashCastleO(board, WKINGSIDE);
		board->castlerights &= ~(WKINGSIDE);
	}
	if (m.to == 56 || m.from == 56){
		hashCastleO(board, BQUEENSIDE);
		board->castlerights &= ~(BQUEENSIDE);
	}
	if (m.to == 63 || m.from == 63){
		hashCastleO(board, BKINGSIDE);
		board->castlerights &= ~(BKINGSIDE);
	}
	
	if (m.from == 4 && mailbox[m.from] == wking){
		hashCastleO(board, WKINGSIDE);
		hashCastleO(board, WQUEENSIDE);
		board->castlerights &= ~(WKINGSIDE);
		board->castlerights &= ~(WQUEENSIDE);
	}
	else if (m.from == 62 && mailbox[m.from] == bking){
		hashCastleO(board, WKINGSIDE);
		hashCastleO(board, WQUEENSIDE);
		board->castlerights &= ~(WKINGSIDE);
		board->castlerights &= ~(WQUEENSIDE);
	}
}

#define DELETEPIECE(p, sq) board->piece[p] &= ~(1LLU << sq)
#define MOVEPIECE(p, sq1, sq2) board->piece[p] ^= ((1LLU << sq1) | (1LLU << sq2))

bitUndo makeMove(bitboard* board, bitMove m){	
	bitUndo undo;
	undo = (bitUndo){board->hashValue, (board->enpassanttarget == 0 ? -1 : __builtin_ctzll(board->enpassanttarget)), mailbox[m.to], board->castlerights};
	
	//TODO: add hash
	
	if (m.flags == 0){
		MOVEPIECE(mailbox[m.from], m.from, m.to);
		board->enpassanttarget = 0;
		hashEnPassantIO(board, 8);
		
		hashPieceIO(board, m.from, mailbox[m.from]);
		hashPieceIO(board, m.to, mailbox[m.from]);
		
		mailbox[m.to] = mailbox[m.from];
	}
	else if (m.flags == CASTLE_FLAG) {
		//		
		MOVEPIECE(mailbox[m.from], m.from, m.to);
		board->enpassanttarget = 0;
		hashEnPassantIO(board, 8);
		
		hashPieceIO(board, m.from, mailbox[m.from]);
		hashPieceIO(board, m.to, mailbox[m.from]);
		
		mailbox[m.to] = mailbox[m.from];
		
		//TODO: rook hash
		
		if (m.from == 4){
			//white
			board->castlerights &= ~(WKINGSIDE | WQUEENSIDE);
			hashCastleO(board, WKINGSIDE);
			hashCastleO(board, WQUEENSIDE);
			if (m.to == 6){
				mailbox[5] = wrook;
				mailbox[7] = -1;
				MOVEPIECE(wrook, 5, 7);
			}
			else {
				mailbox[3] = wrook;
				mailbox[0] = -1;
				MOVEPIECE(wrook, 3, 0);
			}
		}
		else {
			board->castlerights &= ~(BKINGSIDE | BQUEENSIDE);
			hashCastleO(board, BKINGSIDE);
			hashCastleO(board, BQUEENSIDE);
			if (m.to == 62){
				mailbox[61] = brook;
				mailbox[63] = -1;
				MOVEPIECE(brook, 61, 63);
			}
			else {
				mailbox[59] = brook;
				mailbox[56] = -1;
				MOVEPIECE(brook, 59, 56);
			}
		}
	}
	else if (m.flags == PROMOTION_FLAG) {
		// no capture, just promotion
		DELETEPIECE(mailbox[m.from], m.from);
		board->piece[m.promotion] |= (1LL << m.to);
		board->enpassanttarget = 0;
		hashEnPassantIO(board, 8);
		
		hashPieceIO(board, m.from, mailbox[m.from]);
		hashPieceIO(board, m.to, mailbox[m.from]);
		
		mailbox[m.to] = m.promotion;
	}
	else if (m.flags == DOUBLE_PAWNMOVE_FLAG) {
		MOVEPIECE(mailbox[m.from], m.from, m.to);
		board->enpassanttarget = (1LL << (m.to > 31 ? m.to+8 : m.to-8));
		hashEnPassantIO(board, m.to % 8);
		
		hashPieceIO(board, m.from, mailbox[m.from]);
		hashPieceIO(board, m.to, mailbox[m.from]);
		
		mailbox[m.to] = mailbox[m.from];
	}
	else if ((m.flags & CAPTURE_FLAG)){
		//
		board->enpassanttarget = 0;;
		hashEnPassantIO(board, 8);
		
		if ((m.flags & EN_PASSANT_FLAG)){
			uint8_t enpasssq = (m.to > 31 ? m.to+8 : m.to-8);
			undo.capturedPiece = mailbox[enpasssq];
			MOVEPIECE(mailbox[m.from], m.from, m.to);
			DELETEPIECE(mailbox[enpasssq], m.to);
			
			hashPieceIO(board, m.from, mailbox[m.from]);
			hashPieceIO(board, m.to, mailbox[m.from]);
			hashPieceIO(board, enpasssq, mailbox[enpasssq]);

			mailbox[enpasssq] = -1;			
			mailbox[m.to] = mailbox[m.from];
		}
		else{
			undo.capturedPiece = mailbox[m.to];
			DELETEPIECE(mailbox[m.to], m.to);
			
			if ((m.flags & PROMOTION_FLAG)){
				DELETEPIECE(mailbox[m.from], m.from);
				board->piece[m.promotion] |= (1LL << m.to);
				
				hashPieceIO(board, m.from, mailbox[m.from]);
				hashPieceIO(board, m.to, mailbox[m.to]);
				hashPieceIO(board, m.to, m.promotion);
		
				mailbox[m.to] = m.promotion;
			}
			else {
				MOVEPIECE(mailbox[m.from], m.from, m.to);
				board->enpassanttarget = 0;
				
				hashPieceIO(board, m.from, mailbox[m.from]);
				hashPieceIO(board, m.to, mailbox[m.from]);
		
				mailbox[m.to] = mailbox[m.from];
			}
		}
	}
	
	setcastlingrights(board, m);

	hashTomove(board);
	
	mailbox[m.from] = -1;
	return undo;
}


void undoMove(bitboard* board, const bitMove m, const bitUndo u){
	bool blackMove = mailbox[m.to] >= bking;

	board->hashValue = u.prevHash;
	board->castlerights = u.prevCastleRights;
	if (u.prevEpSquareIndex == -1) { board->enpassanttarget = 0; }
	else { board->enpassanttarget = 1LLU << u.prevEpSquareIndex; }
	
	if (m.flags & PROMOTION_FLAG) {
		DELETEPIECE(m.promotion, m.to);
		board->piece[blackMove ? bpawn : wpawn] |= 1LLU << m.from;
		
		mailbox[m.from] = blackMove ? bpawn : wpawn;
	}
	else {
		MOVEPIECE(mailbox[m.to], m.to, m.from);
		mailbox[m.from] = mailbox[m.to];
	}
	
	mailbox[m.to] = -1;		
	
	if (m.flags & CASTLE_FLAG) {
		if (m.from == 4){
			if (m.to == 6){
				mailbox[7] = wrook;
				mailbox[5] = -1;
				MOVEPIECE(wrook, 5, 7);
			}
			else {
				mailbox[0] = wrook;
				mailbox[3] = -1;
				MOVEPIECE(wrook, 3, 0);
			}
		}
		else {
			if (m.to == 62){
				mailbox[63] = brook;
				mailbox[61] = -1;
				MOVEPIECE(brook, 61, 63);
			}
			else {
				mailbox[56] = brook;
				mailbox[59] = -1;
				MOVEPIECE(brook, 59, 56);
			}
		}
	}
	
	if (u.capturedPiece != -1) {
		if (m.flags & EN_PASSANT_FLAG){
			uint8_t enpasssq = (m.to > 31 ? m.to+8 : m.to-8);
			mailbox[enpasssq] = u.capturedPiece;
			board->piece[u.capturedPiece] |= 1LLU << enpasssq;
		}
		else {
			board->piece[u.capturedPiece] |= 1LLU << m.to;
			mailbox[m.to] = u.capturedPiece;
		}
	}
	
	//~ if (m.flags == 0 || m.flags == DOUBLE_PAWNMOVE_FLAG){
		//~ no special move
	//~ }
}
