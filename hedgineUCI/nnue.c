#include "headers/nnue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * This is the part that will later be updated incrementally on make/unmake
 * instead of being rebuilt on every evaluation.
 */
static void refreshAccumulator(Accumulator *acc, const bitboard *board){
	memcpy(acc->v[white], net.ftBias, sizeof net.ftBias);
	memcpy(acc->v[black], net.ftBias, sizeof net.ftBias);

	for (int i = 0; i < NNUE_HIDDEN; i++){
		for (int p = 0; p < 12; p++){
			u64 curr = board->piece[p];
			while (curr) {
				int sq = __builtin_ctzll(curr);
				curr &= curr - 1;

				acc->v[white][i] += net.ftWeight[featureWhite(p, sq)][i];
				acc->v[black][i] += net.ftWeight[featureBlack(p, sq)][i];
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
 */
float NNUEevalFloat(const bitboard *board, sidetomove tomove){
	Accumulator acc;
	refreshAccumulator(&acc, board);

	const float *us = acc.v[tomove];
	const float *them = acc.v[!tomove];

	float out = net.outBias;
	for (int i = 0; i < NNUE_HIDDEN; i++){
		out += crelu(us[i]) * net.outWeight[i];
		out += crelu(them[i]) * net.outWeight[NNUE_HIDDEN + i];
	}

	return out * net.scale;
}

int NNUEeval(const bitboard *board, sidetomove tomove){
	float cp = NNUEevalFloat(board, tomove);
	return (int)(cp >= 0 ? cp + 0.5f : cp - 0.5f);
}

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
