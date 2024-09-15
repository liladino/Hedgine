#include "moves.h"

/*atalakit egy mezot mezoszamokka
 */
squarenums makesquarenum(square a){
	squarenums b;
	b.file = a.file - 'a' + 2;
	b.rank = a.rank + 1;
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

/* movelist *head = createMove(m); 
 * makes a new move list
 * */
movelist *createMove(move m) {
	movelist *newMove = (movelist*)malloc(sizeof(movelist));
	if (newMove == NULL) {
		printf(TXT_RED "Memory allocation failed\n"  DEFAULT);
		exit(1);
	}
	newMove->m = m;
	newMove->next = NULL;
	return newMove;
}

/* movelist *head; move m;
 * firstmove(&head, m); 
 * push a move to the front
 * */
void firstmove(movelist **head, move m) { 
	movelist *newNode = createMove(m); 
	newNode->next = *head; 
	*head = newNode;
}

/* move m; movelist *head = createMove(m);
 * insertMove(head, m); 
 * push a move to the end
 * **NEEDS A STARTIN NODE!** */
void insertMove(movelist *head, move m) {
	movelist *newMove = createMove(m);
	movelist *i = head;
	movelist *tail = head;
	while (i != NULL) {
		i = i->next;
		if (i != NULL)
			tail = i;
	}
	tail->next = newMove;
}

/* movelist *head;
 * undoMove(&head); 
 * pops the last move in the list
 * */
void undoMove(movelist **head) {
	if (*head == NULL)  
		return;
	if ((*head)->next == NULL)  // startnode is a nullmove
		return;

	movelist *tail = *head;
	movelist *previous = NULL;
	
    while (tail->next != NULL) {
		previous = tail;
		tail = tail->next;
	}
	
	previous->next = NULL;	//az utolso elotti mutatojat beallitjuk NULL-ra
	free(tail);				//az utolsot felszabaditjuk
}

/* movelist *head;
 * printmoves(head); 
 * */
void printmoves(movelist *head) {
	movelist *current = head;
	current = current->next; //a startnode nem lepes
	while (current != NULL) {
		printmove(current->m);
		current = current->next;
	}
	printf("*\n");
}

void freemoves(movelist *head) {
	while (head != NULL) {
		movelist *temp = head;
		head = head->next;
		free(temp);
	}
}

int countmoves(movelist *head) {
	movelist* a = head;
	int i = -1;
	while (head != NULL) {
		a = a->next;
		i++;
	}
	return i;
}


/* BITS
 * 
 * The result of a bitboard method is a bitboard too, so we can just take that instead of moves, and in the end convert it to a move
 * This way, printing out the moves doesn't make much sense - we don't save any moves, but boards;
 * however out of convinience ill call them moves, because they are technically moves 
 * */
 
int isMoveInMoveArray(const bitboard* const board, const movearray* legalmoves, const bool tomove, const move m){
	for (int i = 0; i < legalmoves->size; i++){
		move curr = boardConvertTomove(board, &legalmoves->boards[i], tomove);
		if (curr.from.file == m.from.file && curr.from.rank == m.from.rank && curr.to.file == m.to.file && curr.to.rank == m.to.rank && m.promotion == curr.promotion){
			return i;
		}
	}
	return -1;
} 

int isMoveLegal(bitboard* board, const bool tomove, const move m){
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, *board, tomove, false);
	int x = isMoveInMoveArray(board, &legalmoves, tomove, m);
	
	if (x == -1){
		return 1;
	}
	
	*board = legalmoves.boards[x];
	
	legalmoves.size = 0;
	bitGenerateLegalmoves(&legalmoves, *board, !tomove, false);
	
	if (legalmoves.size == 0) return 2;
	
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
