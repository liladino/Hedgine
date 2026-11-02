#include "headers/movepicker.h"

/*
 * Order moves
 * */
typedef struct pair {
	int eval, index;
} pair;

void insertionSort(pair a[], int n) {
	for (int i = 1; i < n; i++) {
		pair keyy = a[i];
		int j = i - 1;
		while (j >= 0 && a[j].eval < keyy.eval) { // DESC
			a[j + 1] = a[j];
			j--;
		}
		a[j + 1] = keyy;
	}
}

bool sameMove(const bitMove* const a, const bitMove* const b) {
	return a->from == b->from &&
		   a->to == b->to &&
		   a->piece == b->piece &&
		   a->promotion == b->promotion &&
		   a->flags == b->flags;
}

/* Promotions, captures and en passant. Quiet moves get 0, so they come after
 * every tactical move (e.g. the quiet evasions in quiescence).
 * */
static int scoreTactical(const bitMove* m) {
	static int pieceValue[12] = { 0, 900, 500, 300, 300, 100, 0, 900, 500, 300, 300, 100 };
	if (m->flags & PROMOTION_FLAG) { return 900000 + pieceValue[m->promotion]; }
	if (m->flags & (EN_PASSANT_FLAG)) { return 800000 + 900; }
	if (m->flags & (CAPTURE_FLAG)) { return 800000 + attackValue(m->from, m->to); }
	return 0;
}

int scoreMove(bitMove* m, const int history[12][64]) {
	if (m->flags & (PROMOTION_FLAG | EN_PASSANT_FLAG | CAPTURE_FLAG)) { return scoreTactical(m); }

	return history[m->piece][m->to];
}

static void sortByScores(movearray* moves, pair moveScores[]){
	insertionSort(moveScores, moves->size);

	movearray copy = *moves;
	for (int i = 0; i < moves->size; i++) {
		moves->array[i] = copy.array[moveScores[i].index];
	}
}

void orderMoves(movearray* moves, const int history[12][64]){
	pair moveScores[MAXMOVECOUNT_INPOS];
	for (int i = 0; i < moves->size; i++) {
		moveScores[i] = (pair){scoreMove(&moves->array[i], history), i};
	}
	sortByScores(moves, moveScores);
}

static void orderEvasions(movearray* moves, const bitMove* const killers[2], const int history[12][64]){
	pair moveScores[MAXMOVECOUNT_INPOS];
	for (int i = 0; i < moves->size; i++) {
		bitMove* m = &moves->array[i];
		int score;
		if (m->flags & (PROMOTION_FLAG | EN_PASSANT_FLAG | CAPTURE_FLAG)) { score = scoreTactical(m); }
		else if (sameMove(m, killers[0])) { score = 700000; }
		else if (sameMove(m, killers[1])) { score = 650000; }
		else { score = history[m->piece][m->to]; }
		moveScores[i] = (pair){score, i};
	}
	sortByScores(moves, moveScores);
}

//quiescence: no killers and history, only the tactical scores
static void orderTacticalMoves(movearray* moves){
	pair moveScores[MAXMOVECOUNT_INPOS];
	for (int i = 0; i < moves->size; i++) {
		moveScores[i] = (pair){scoreTactical(&moves->array[i]), i};
	}
	sortByScores(moves, moveScores);
}

void initMovePicker(
	MovePicker *picker,
	const bitMove *hint,
	const bitMove *killer0,
	const bitMove *killer1,
	const int history[12][64],
	bool inCheck,
	bool quiescence)
{	
	picker->stage = PICK_HINT;
	picker->hint = *hint;
	picker->killers[0] = killer0;
	picker->killers[1] = killer1;
	picker->hintReturned = false;
	picker->inCheck = inCheck;
	picker->quiescence = quiescence;
	picker->history = history;
	picker->moves.size = 0;
	picker->next = 0;
}

bool nextMove(MovePicker *picker, const bitboard *board, bool tomove, bitMove *out) {
	if (picker->stage == PICK_DONE) {
		return false;
	}

	if (PICK_HINT == picker->stage) {
		picker->next = 0;
		picker->stage = PICK_TACTICAL; 
		
		if (!picker->quiescence && isPseudoLegal(board, tomove, &picker->hint)){
			*out = picker->hint;
			picker->hintReturned = true;
			return true;
		}
	}

	if (picker->inCheck){
		if (0 == picker->next) {
			generateMovesWithModes(&picker->moves, board, tomove, GEN_EVASIONS);
			if (picker->quiescence){
				orderTacticalMoves(&picker->moves);
			}
			else{
				orderEvasions(&picker->moves, picker->killers, picker->history);
			}
		}
		
		while (picker->next < picker->moves.size){
			if (picker->hintReturned && sameMove(&picker->moves.array[picker->next], &picker->hint)){
				picker->next++;
			}
			else{
				*out = picker->moves.array[picker->next++];
				return true;
			}
		}
		picker->stage = PICK_DONE;
		return false;
	}
	
	if (PICK_TACTICAL == picker->stage) {
		if (0 == picker->next) {
			generateMovesWithModes(&picker->moves, board, tomove, GEN_TACTICAL);
			if (picker->quiescence){
				orderTacticalMoves(&picker->moves);
			}
			else{
				orderMoves(&picker->moves, picker->history);
			}
		}
		
		while (picker->next < picker->moves.size){
			if (picker->hintReturned && sameMove(&picker->moves.array[picker->next], &picker->hint)){
				picker->next++;
			}
			else{
				*out = picker->moves.array[picker->next++];
				return true;
			}
		}
		picker->stage = PICK_KILLER1;
		picker->next = 0;
		if (picker->quiescence) {
			picker->stage = PICK_DONE;
			return false;
		}
	}

	if (PICK_KILLER1 == picker->stage){
		picker->stage = PICK_KILLER2; 
		
		if (isPseudoLegal(board, tomove, picker->killers[0])){
			if (!sameMove(picker->killers[0], &picker->hint)){
				*out = *picker->killers[0];
				return true;
			}
		}
	}

	if (PICK_KILLER2 == picker->stage){
		picker->stage = PICK_QUIET; 
		
		if (isPseudoLegal(board, tomove, picker->killers[1])){
			if (!sameMove(picker->killers[1], &picker->hint)){
				*out = *picker->killers[1];
				return true;
			}
		}
	}

	if (PICK_QUIET == picker->stage) {
		if (0 == picker->next) {
			generateMovesWithModes(&picker->moves, board, tomove, GEN_QUIET);
			orderMoves(&picker->moves, picker->history);
		}
		
		while (picker->next < picker->moves.size){
			if (sameMove(&picker->moves.array[picker->next], &picker->hint)){
				picker->next++;
			}
			else if (sameMove(&picker->moves.array[picker->next], picker->killers[0])){
				picker->next++;
			}
			else if (sameMove(&picker->moves.array[picker->next], picker->killers[1])){
				picker->next++;
			}
			else{
				*out = picker->moves.array[picker->next++];
				return true;
			}
		}
		picker->stage = PICK_DONE;
		return false;
	}

	return false;
}