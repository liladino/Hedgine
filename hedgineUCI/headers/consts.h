#ifndef CONSTS_H
#define CONSTS_H

#include <stdint.h>
#include <stdbool.h>

/* By dumb luck, I arrived at this upper bound, tho it turns out, it is actually 
 * the lowest upper bound, as discussed here: 
 * https://lichess.org/@/Tobs40/blog/why-a-reachable-position-can-have-at-most-218-playable-moves/a5xdxeqs
 * (My methodes were a lot less elegant, and just assumed an illegal position)
 * 
 * Tho statistical analysis by others shows, that on average a game has at most 
 * 40 legal moves in a single turn, 
 * https://chess.stackexchange.com/questions/23135/what-is-the-average-number-of-legal-moves-per-turn
 * it isn't much of an improvement in speed if we lower this number.
 * */
#define MAXMOVECOUNT_INPOS 218

/* A pseudorandom value, that indicates, that as a return value signals, that 
 * there is no hash match.  
 * */
#define NO_HASH_ENTRY 21454837

/* Minimal and maximal size of the transposition table. The maximum size 
 * depends on, whether or not the program was compiled in debug mode.
 * */
#define TT_MIN_SIZE_MB 1

#ifndef DEBUG
	#define TT_DEF_SIZE_MB 12
	#define TT_MAX_SIZE_MB 128
#else
	#define TT_DEF_SIZE_MB 1
	#define TT_MAX_SIZE_MB 2
#endif

#define HASHING_ENABLED 1
#define REPETITION_TABLE_SIZE 1024

typedef enum resultconst{
	WHITEWON =  2147400000,
	BLACKWON = -2147400000,
	DRAW = 0,
	ONGOING = -1
	//~ stalemate = 200000
}resultconst;

typedef enum sidetomove{
	white = 0,
	black = 1
}sidetomove;

typedef enum piecenums{
	wking = 0,
	wqueen = 1,
	wrook = 2,
	wbishop = 3,
	wknight = 4,
	wpawn = 5,
	bking = 6,
	bqueen = 7,
	brook = 8,
	bbishop = 9,
	bknight = 10,
	bpawn = 11
}piecenums;

typedef enum raydir{
	/*
	 * noWe d4     e5 nort     f5  noEa
	 *         +7    +8    +9
	 *             \  |  /
	 * west c4 -1 <- e4 -> +1  f4  east
	 *             /  |  \
	 *         -9    -8    -7
	 * soWe c3     e3 sout     f3  soEa
	 * 
	 * */
	 // +  shift dir: <<
	 noWe = 7,
	 nort = 8,
	 noEa = 9,
	 east = 1,
	 // -  shift dir: >>
	 west = 1,
	 soWe = 9,
	 sout = 8,
	 soEa = 7
}raydir;

typedef enum castleRightsFlag {
	WKINGSIDE = 1,
	WQUEENSIDE = 2,
	BKINGSIDE = 4,
	BQUEENSIDE = 8
} castleRightsFlag;

typedef enum bitMoveFlags{
	CAPTURE_FLAG = 1,
	CASTLE_FLAG = 2,
	EN_PASSANT_FLAG = 4,
	PROMOTION_FLAG = 8,
	DOUBLE_PAWNMOVE_FLAG = 16
} bitMoveFlags;

typedef enum evalflag{
	EXACT_EVAL_FLAG = 1, 
	LOWER_BOUND_FLAG = 2, 
	UPPER_BOUND_FLAG = 3, 
	LAST_BEST_EVAL_FLAG = 4
}evalflag;

#endif