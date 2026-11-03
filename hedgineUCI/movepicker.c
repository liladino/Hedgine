#include "headers/movepicker.h"

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

int scoreMove(bitMove* m, const bitMove* const killers[2], const int history[12][64]) {
	if (m->flags & (PROMOTION_FLAG | EN_PASSANT_FLAG | CAPTURE_FLAG)) { return scoreTactical(m); }

	if (sameMove(m, killers[0])) return 700000;
	if (sameMove(m, killers[1])) return 650000;

	return history[m->piece][m->to];
}

static void scoreMoves(MovePicker* p, bool tacticalOnly) {
	for (int i = 0; i < p->moves.size; i++) {
		p->scores[i] = tacticalOnly  
			? scoreTactical(&p->moves.array[i])
			: scoreMove(&p->moves.array[i], p->killers, p->history);
	}
}

/*
 * Staged selection sort, at each move it selects the best move out of the 
 * remaining
 */
static bool pickNext(MovePicker* p, bitMove* out) {
	while (p->next < p->moves.size) {
		int best = p->next;
		for (int j = p->next + 1; j < p->moves.size; j++) {
			if (p->scores[j] > p->scores[best]) best = j;
		}
		if (best != p->next) {
			//cascade the best to the front, keeping the order of the rest (stable)
			int ts = p->scores[best];
			bitMove tm = p->moves.array[best];
			for (int j = best; j > p->next; j--) {
				p->scores[j] = p->scores[j - 1];
				p->moves.array[j] = p->moves.array[j - 1];
			}
			p->scores[p->next] = ts;
			p->moves.array[p->next] = tm;
		}
		const bitMove* m = &p->moves.array[p->next++];
		if (p->hintReturned && sameMove(m, &p->hint)) {
			continue;
		}
		*out = *m;
		return true;
	}
	return false;
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
			scoreMoves(picker, picker->quiescence);
		}
		
		if (pickNext(picker, out)) {
			return true;
		}
		
		picker->stage = PICK_DONE;
		return false;
	}

	if (PICK_TACTICAL == picker->stage) {
		if (0 == picker->next) {
			generateMovesWithModes(&picker->moves, board, tomove, GEN_TACTICAL);
			scoreMoves(picker, true);
		}
		
		if (pickNext(picker, out)) {
			return true;
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
			scoreMoves(picker, false);
		}

		if (pickNext(picker, out)) {
			return true;
		}
		
		picker->stage = PICK_DONE;
		return false;
	}

	return false;
}