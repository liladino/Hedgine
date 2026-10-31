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

int scoreMove(bitMove* m, const bitMove* const killers[2], const int history[12][64]) {
	static int pieceValue[12] = { 0, 900, 500, 300, 300, 100, 0, 900, 500, 300, 300, 100 };
	if (m->flags & PROMOTION_FLAG) { return 900000 + pieceValue[m->promotion]; }
	if (m->flags & (EN_PASSANT_FLAG)) { return 800000; }
	if (m->flags & (CAPTURE_FLAG)) { return 800000 + attackValue(m->from, m->to); }
	
	if (sameMove(m, killers[0])) return 700000;
	if (sameMove(m, killers[1])) return 650000;
	
	return history[m->piece][m->to];
}

void orderMoves(movearray* moves, const bitMove* const killers[2], const int history[12][64]){
	pair moveScores[MAXMOVECOUNT_INPOS];
	for (int i = 0; i < moves->size; i++) {
		moveScores[i] = (pair){scoreMove(&moves->array[i], killers, history), i};
	}
	insertionSort(moveScores, moves->size);
	
	movearray copy = *moves;
	for (int i = 0; i < moves->size; i++) {
		moves->array[i] = copy.array[moveScores[i].index];
	}
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
			if (!picker->quiescence){
				orderMoves(&picker->moves, picker->killers, picker->history);
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
			if (!picker->quiescence){
				orderMoves(&picker->moves, picker->killers, picker->history);
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
		picker->stage = PICK_QUIET;
		picker->next = 0;
		if (picker->quiescence) {
			return false;
		}
	}

	if (PICK_QUIET == picker->stage) {
		if (picker->quiescence){
			picker->stage = PICK_DONE;
			return false;
		}
		if (0 == picker->next) {
			generateMovesWithModes(&picker->moves, board, tomove, GEN_QUIET);
			orderMoves(&picker->moves, picker->killers, picker->history);
		}
		
		while (picker->next < picker->moves.size){
			if (sameMove(&picker->moves.array[picker->next], &picker->hint)){
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