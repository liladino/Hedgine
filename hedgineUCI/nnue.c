#include "headers/nnue.h"
#ifdef NNUE_INCREMENTAL
	#include "headers/moves.h"
	#include "headers/legalmoves.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static inline int i_max(int a, int b){
	return (a > b ? a : b);
}

/**
 * 768 -> 128x2 -> 1 perspective network, float version.
 * The network and the file format are described at the top of nnue/train.py.
 *
 *   accumulator[view] = ftBias + sum of ftWeight[feature] over all pieces
 *   out = outBias + sum crelu(us[i]) * outWeight[i] + sum crelu(them[i]) * outWeight[H + i]
 *   eval = out * scale     (centipawns, side to move's POV)
 */
typedef struct Network {
	float ftWeight[NNUE_FEATURES][NNUE_HIDDEN]; /* row f: what feature f adds to the accumulator */
	float ftBias[NNUE_HIDDEN];
	float outWeight[2 * NNUE_HIDDEN];           /* first half: side to move's accumulator */
	float outBias;
	int scale;
} Network;

/**
 * one accumulator per view, indexed with sidetomove: v[white], v[black] 
 */
typedef struct Accumulator {
	float v[2][NNUE_HIDDEN];
} Accumulator;

static Network net;

/**
 * Loading
 *
 * The embedded net and a net file both go through parseNet(); a file is read
 * into memory first. The bytes are copied out with memcpy, because an embedded
 * byte array has no float alignment.
 */
typedef struct Reader {
	const unsigned char *data;
	size_t size;
	size_t pos;
} Reader;

static bool take(Reader *r, void *dst, size_t bytes, const char *what){
	if (r->size - r->pos < bytes){
		printf("info string NNUE: data ended while reading %s\n", what);
		return false;
	}
	memcpy(dst, r->data + r->pos, bytes);
	r->pos += bytes;
	return true;
}

static bool parseNet(const unsigned char *data, size_t size, const char *name){
	/* parsed into a copy first, so a bad file can't leave the current net half overwritten */
	static Network tmp;
	Reader r = {data, size, 0};

	/**
	 * header: "HNNU", version, hidden size, scale (uint32, little endian)
	 */
	char magic[4];
	uint32_t version, hidden, scale;
	bool ok = take(&r, magic, sizeof magic, "header")
		&& take(&r, &version, sizeof version, "header")
		&& take(&r, &hidden, sizeof hidden, "header")
		&& take(&r, &scale, sizeof scale, "header");

	if (ok && memcmp(magic, "HNNU", 4) != 0){
		printf("info string NNUE: %s is not a net file\n", name);
		ok = false;
	}
	if (ok && version != 1){
		printf("info string NNUE: unsupported version %u\n", (unsigned)version);
		ok = false;
	}
	if (ok && hidden != NNUE_HIDDEN){
		printf("info string NNUE: net has %u hidden neurons, engine compiled for %d\n",
			(unsigned)hidden, NNUE_HIDDEN);
		ok = false;
	}

	/**
	 * the arrays are stored in the same order and layout as the struct fields
	 */
	ok = ok
		&& take(&r, tmp.ftWeight, sizeof tmp.ftWeight, "feature weights")
		&& take(&r, tmp.ftBias, sizeof tmp.ftBias, "feature biases")
		&& take(&r, tmp.outWeight, sizeof tmp.outWeight, "output weights")
		&& take(&r, &tmp.outBias, sizeof tmp.outBias, "output bias");

	if (ok && r.pos != r.size){
		printf("info string NNUE: unexpected data at the end of %s\n", name);
		ok = false;
	}

	if (ok){
		tmp.scale = (int)scale;
		net = tmp;
		printf("info string NNUE: loaded %s\n", name);
	}
	return ok;
}

bool NNUEload(const char *path){
	FILE *f = fopen(path, "rb");
	if (f == NULL){
		printf("info string NNUE: cannot open %s\n", path);
		return false;
	}

	unsigned char *data = NULL;
	long size = -1;
	if (fseek(f, 0, SEEK_END) == 0 && (size = ftell(f)) >= 0 && fseek(f, 0, SEEK_SET) == 0){
		data = malloc(size > 0 ? (size_t)size : 1);
	}

	bool ok = data != NULL && fread(data, 1, (size_t)size, f) == (size_t)size;
	if (!ok){
		printf("info string NNUE: cannot read %s\n", path);
	}
	fclose(f);

	ok = ok && parseNet(data, (size_t)size, path);
	free(data);
	return ok;
}

/**
 * The net the makefile embedded at build time (NNUE_EMBED_FILE).
 * #embed path is relative to this source file
 */
#ifdef NNUE_EMBED_FILE
static const unsigned char embeddedNet[] = {
	#embed NNUE_EMBED_FILE
};

bool NNUEloadEmbedded(void){
	return parseNet(embeddedNet, sizeof embeddedNet, "embedded net " NNUE_EMBED_FILE);
}
#else
bool NNUEloadEmbedded(void){
	printf("info string NNUE: no net was embedded at build time\n");
	return false;
}
#endif

/** 
 * Features
 */

/**
 * Index of a piece among the 768 inputs, as seen by white.
 * piece: piecenums (wking = 0 ... bpawn = 11), square: a1 = 0 ... h8 = 63.
 * Matches features() in nnue/convert.py
 */
static inline int featureWhite(int piece, int square){
	return piece * 64 + square;
}

/**
 * Index of a piece among the 768 inputs, as seen by black.
 * Matches BLACK_LUT in nnue/train.py.
 */
static inline int featureBlack(int piece, int square){
	return ((piece + 6) % 12) * 64 + (square ^ 56);
}

/** 
 * Accumulator
 */

/**
 * Builds both accumulators from scratch.
 * With NNUE_INCREMENTAL this only runs at the root of the search (and in the
 * checks)
 */
static void refreshAccumulator(Accumulator *acc, const bitboard *board){
	memcpy(acc->v[white], net.ftBias, sizeof net.ftBias);
	memcpy(acc->v[black], net.ftBias, sizeof net.ftBias);

	for (int p = 0; p < 12; p++){
		u64 curr = board->piece[p];
		while (curr) {
			int sq = __builtin_ctzll(curr);
			curr &= curr - 1;

			const float *w = net.ftWeight[featureWhite(p, sq)];
			const float *b = net.ftWeight[featureBlack(p, sq)];
			for (int i = 0; i < NNUE_HIDDEN; i++){
				acc->v[white][i] += w[i];
				acc->v[black][i] += b[i];
			}
		}
	}
}


/** 
 * Forward pass
 */

/** 
 * clipped ReLU, clamp x to [0, 1]
 */
static inline float crelu(float x){
	if (0 > x) return 0;
	if (1 < x) return 1;
	return x;
}

/**
 * h = clamp([acc_us, acc_them], 0, 1)
 * out = h . w_out + b_out
 * eval (centipawns, side to move's POV) = out * SCALE
 *
 * Only this part depends on the side to move: the accumulators themselves are
 * the same whoever is to move, which is what makes them easy to update.
 */
static float forward(const Accumulator *acc, sidetomove tomove){
	const float *us = acc->v[tomove];
	const float *them = acc->v[!tomove];

	float out = net.outBias;
	for (int i = 0; i < NNUE_HIDDEN; i++){
		out += crelu(us[i]) * net.outWeight[i];
		out += crelu(them[i]) * net.outWeight[NNUE_HIDDEN + i];
	}

	return out * net.scale;
}

/**
 * Always rebuilding the accumulator from the board
 */
float NNUEevalFloat(const bitboard *board, sidetomove tomove){
	Accumulator acc;
	refreshAccumulator(&acc, board);
	return forward(&acc, tomove);
}

static inline int roundCp(float cp){
	return (int)(cp >= 0 ? cp + 0.5f : cp - 0.5f);
}

#ifndef NNUE_INCREMENTAL

int NNUEeval(const bitboard *board, sidetomove tomove){
	return roundCp(NNUEevalFloat(board, tomove));
}

#else

/**
 * Incremental accumulator
 *
 * Idea: the accumulator is a sum over the active features (one per piece on
 * the board). A move only turns a few features on or off, so instead of
 * summing all ~32 pieces again, we start from the parent position's
 * accumulator and add / subtract the columns of the few features that changed.
 *
 * Unmaking is free: we never modify the parent, we work on a copy one slot
 * higher in the stack. Popping just moves the index back down.
 *
 *   s_accStack[0]            root position (built with refreshAccumulator)
 *   s_accStack[1]            after the 1st move of the current line
 *   ...
 *   s_accStack[s_accTop]     the position s_searchBoard is currently in
 */

/* search plies + quiescence plies (MAXPLY in engine.c is 80), with some slack */
#define NNUE_STACK_SIZE 128

static Accumulator s_accStack[NNUE_STACK_SIZE];
static int s_accTop = 0;
static bool s_accActive = false;

/**
 * Turns on the feature "piece on square" in both views.
 */
static inline void accAdd(Accumulator *acc, int piece, int square){
	const float *w = net.ftWeight[featureWhite(piece, square)];
	const float *b = net.ftWeight[featureBlack(piece, square)];
	for (int i = 0; i < NNUE_HIDDEN; i++){
		acc->v[white][i] += w[i];
		acc->v[black][i] += b[i];
	}
}

/**
 * Turns off the feature "piece on square" in both views.
 */
static inline void accSub(Accumulator *acc, int piece, int square){
	const float *w = net.ftWeight[featureWhite(piece, square)];
	const float *b = net.ftWeight[featureBlack(piece, square)];
	for (int i = 0; i < NNUE_HIDDEN; i++){
		acc->v[white][i] -= w[i];
		acc->v[black][i] -= b[i];
	}
}

void NNUEbeginSearch(const bitboard *board){
	refreshAccumulator(&s_accStack[0], board);
	s_accTop = 0;
	s_accActive = true;
}

void NNUEendSearch(void){
	s_accActive = false;
}

/**
 * Call right after a makeMove that turned out to be legal.
 *
 * m->piece      moved piece (pawn if promotion)
 * m->from/to    squares, a1 = 0 ... h8 = 63, same as the features
 * m->promotion  new piece (piecenums), if PROMOTION_FLAG
 * u->capturedPiece  the captured piece (piecenums), -1 if none.
 *                   For en passant it's the captured pawn, but it is NOT on m->to.
 *
 * Look at makeMove() in moves.c: every board->piece[] bit it flips must turn
 * into exactly one accAdd or accSub here.
 */
void NNUEpush(const bitMove *m, const bitUndo *u){
	#ifdef DEBUG
	if (s_accTop + 1 >= NNUE_STACK_SIZE){
		printf("info string NNUE: accumulator stack overflow\n");
		return;
	}
	#endif

	const Accumulator *parent = &s_accStack[s_accTop];
	Accumulator *acc = &s_accStack[++s_accTop];
	*acc = *parent;

	const int piece = m->piece;

	if (m->flags == 0 || m->flags == DOUBLE_PAWNMOVE_FLAG){
		// quiet move 
		accSub(acc, piece, m->from);
		accAdd(acc, piece, m->to);
	}
	else if (m->flags & CASTLE_FLAG){
		accSub(acc, piece, m->from);
		accAdd(acc, piece, m->to);
		//rook
		if (m->from == 4) {
			// white
			if (m->to == 6) {
				accSub(acc, wrook, 7);
				accAdd(acc, wrook, 5);
			}
			else {
				accSub(acc, wrook, 0);
				accAdd(acc, wrook, 3);
			}
		}
		else {
			if (m->to == 62) {
				accSub(acc, brook, 63);
				accAdd(acc, brook, 61);
			}
			else {
				accSub(acc, brook, 56);
				accAdd(acc, brook, 59);
			}
		}
	}
	else if (m->flags & EN_PASSANT_FLAG){
		accSub(acc, piece, m->from);
		accAdd(acc, piece, m->to);
		uint8_t enpasssq = (m->to > 31 ? m->to - 8 : m->to + 8);
		accSub(acc, u->capturedPiece, enpasssq);
	}
	else if (m->flags & PROMOTION_FLAG){
		accSub(acc, piece, m->from);
		accAdd(acc, m->promotion, m->to);
		if (m->flags & CAPTURE_FLAG){
			accSub(acc, u->capturedPiece, m->to);	
		}
	}
	else if (m->flags & CAPTURE_FLAG){
		accSub(acc, piece, m->from);
		accAdd(acc, piece, m->to);
		accSub(acc, u->capturedPiece, m->to);
	}

	/**
	 * TODO: every case above is 2-4 separate passes over
	 * the 128 neurons, plus one more for the copy. A fused
	 * accSubAdd(parent, acc, ...) that writes acc = parent - a + b in a single
	 * loop avoids the copy and touches the memory once. 
	 */
}

void NNUEpop(void){
	#ifdef DEBUG
	if (s_accTop <= 0){
		printf("info string NNUE: accumulator stack underflow\n");
		return;
	}
	#endif
	s_accTop--;
}

/**
 * A float accumulator built by + and - drifts a little from the one summed in a
 * different order, so the comparison has a tolerance. (An int16 quantized
 * accumulator would be exact.)
 */
static bool sameAccumulator(const Accumulator *a, const Accumulator *b, float *maxDiff){
	*maxDiff = 0;
	for (int c = 0; c < 2; c++){
		for (int i = 0; i < NNUE_HIDDEN; i++){
			float d = a->v[c][i] - b->v[c][i];
			if (d < 0) d = -d;
			if (d > *maxDiff) *maxDiff = d;
		}
	}
	return *maxDiff < 1e-3f;
}

int NNUEeval(const bitboard *board, sidetomove tomove){
	if (!s_accActive){
		/* outside of a search (tests, eval command): nothing to update from */
		return roundCp(NNUEevalFloat(board, tomove));
	}

	#ifdef DEBUG
	// check if the incremental accumulator differs from a fresh one
	Accumulator fresh;
	refreshAccumulator(&fresh, board);
	float maxDiff;
	if (!sameAccumulator(&s_accStack[s_accTop], &fresh, &maxDiff)){
		printf("info string NNUE: incremental accumulator differs by %f at ply %d\n", maxDiff, s_accTop);
	}
	#endif

	return roundCp(forward(&s_accStack[s_accTop], tomove));
}

/**
 * Test: random walks, compared with a rebuild after every push and every pop
 */

/* tiny deterministic PRNG, so a failure can be reproduced */
static u64 testRand(u64 *state){
	*state ^= *state << 13;
	*state ^= *state >> 7;
	*state ^= *state << 17;
	return *state;
}

static const char *moveKind(const bitMove *m){
	if (m->flags & CASTLE_FLAG) return "castling";
	if (m->flags & EN_PASSANT_FLAG) return "en passant";
	if ((m->flags & PROMOTION_FLAG) && (m->flags & CAPTURE_FLAG)) return "capture promotion";
	if (m->flags & PROMOTION_FLAG) return "promotion";
	if (m->flags & CAPTURE_FLAG) return "capture";
	return "quiet";
}

bool NNUEincrementalTest(void){
	static const char *fens[] = {
		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
		"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
		"rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
		"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",
		"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
	};
	const int walks = 200, maxLen = 60;

	u64 seed = 0x9E3779B97F4A7C15ULL;
	int checked = 0, failed = 0;

	for (size_t f = 0; f < sizeof fens / sizeof fens[0]; f++){
		for (int w = 0; w < walks; w++){
			bitboard board;
			sidetomove tomove;
			int fmv, movenum;
			if (setboardFEN(fens[f], &board, &tomove, &fmv, &movenum) != 0){
				printf("bad FEN: %s\n", fens[f]);
				return false;
			}
			NNUEbeginSearch(&board);

			bitMove played[64];
			bitUndo undos[64];
			int len = 0;

			Accumulator fresh;
			float maxDiff;
			bool walkOk = true;

			/* forward: make + push, compare */
			while (len < maxLen && walkOk){
				movearray moves;
				generateLegalMoves(&moves, &board, tomove);
				if (moves.size == 0){
					break;
				}
				bitMove m = moves.array[testRand(&seed) % moves.size];

				undos[len] = makeMove(&board, &m);
				played[len++] = m;
				NNUEpush(&m, &undos[len - 1]);
				tomove = !tomove;

				refreshAccumulator(&fresh, &board);
				checked++;
				if (!sameAccumulator(&s_accStack[s_accTop], &fresh, &maxDiff)){
					printf("FAIL after a(n) %s move (off by %f), fen %s, move %d:\n", moveKind(&m), maxDiff, fens[f], len);
					for (int k = i_max(len-3, 0); k < len; k++){
						printf("%3d: ", k);
						printBitMove(stdout, played[k]);
						printf("\n");
					}
					printf("\n");
					walkOk = false;
				}
			}

			/* backward: undo + pop, compare */
			while (len > 0 && walkOk){
				len--;
				undoMove(&board, &played[len], &undos[len]);
				NNUEpop();
				tomove = !tomove;

				refreshAccumulator(&fresh, &board);
				checked++;
				if (!sameAccumulator(&s_accStack[s_accTop], &fresh, &maxDiff)){
					printf("FAIL after popping a %s move (off by %f), fen %s\n", moveKind(&played[len]), maxDiff, fens[f]);
					walkOk = false;
				}
			}

			failed += !walkOk;
			NNUEendSearch();
		}
	}

	printf("%d positions checked, %d of %d walks failed\n",
		checked, failed, (int)(walks * (sizeof fens / sizeof fens[0])));
	return failed == 0;
}

#endif /* NNUE_INCREMENTAL */

/** 
 * Test against the Python reference
 */
bool NNUEgoldenTest(const char *goldenPath){
	FILE *f = fopen(goldenPath, "r");
	if (f == NULL){
		printf("cannot open %s\n", goldenPath);
		return false;
	}

	char line[256];
	int tested = 0, failed = 0;
	while (fgets(line, sizeof line, f)){
		char *bar = strrchr(line, '|');
		if (bar == NULL){
			continue;
		}
		*bar = '\0';
		float expected = (float)atof(bar + 1);

		bitboard board;
		sidetomove tomove;
		int fmv, movenum;
		if (setboardFEN(line, &board, &tomove, &fmv, &movenum) != 0){
			printf("bad FEN: %s\n", line);
			failed++;
			continue;
		}

		float got = NNUEevalFloat(&board, tomove);
		/* float sums in a different order than PyTorch: tiny differences are expected */
		float diff = got - expected;
		bool ok = diff < 0.05f && diff > -0.05f;
		printf("%s %10.4f (python %10.4f)  %s\n", ok ? "ok  " : "FAIL", got, expected, line);
		tested++;
		failed += !ok;
	}
	fclose(f);

	printf("%d positions, %d failed\n", tested, failed);
	return tested > 0 && failed == 0;
}
