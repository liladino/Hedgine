#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* These were only relevant when the program had its own CLI. */
#define BG_BLACK "\x1b[40m"
#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"
#define TXT_BLACK "\x1b[30m"
#define TXT_RED "\x1b[31m"
#define TXT_GREEN "\x1b[32m"
#define TXT_WHITE "\x1b[37m"
#define TXT_BOLD "\x1B[1m"
#define DEFAULT "\x1b[37m" "\x1b[m"
#define CLEAR "\033[2J\033[H"

/* By dumb luck, I arrived at this upper bound, tho it turns out, it is actually 
 * the lowest upper bound, as discussed here: 
 * https://lichess.org/@/Tobs40/blog/why-a-reachable-position-can-have-at-most-218-playable-moves/a5xdxeqs
 * (My methodes were a lot less elegant, and just assumed an illegal position)
 * 
 * Tho statistical analysis by others shows, that on average a game has at most 
 * 40 legal moves in a single turn, 
 * https://chess.stackexchange.com/questions/23135/what-is-the-average-number-of-legal-moves-per-turn
 * it isn't much of an improvement in speed if we lower this number - 
 * and i didn't even took into account the reallocation cost if we would actually 
 * have a position that's over the smaller max-count
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

typedef unsigned long long u64;


typedef struct square{
	char file;
	int rank; 
} square;

typedef struct squarenums{
	int file;
	int rank;
} squarenums;

typedef struct move{
	squarenums from;
	squarenums to;
	char promotion;
} move;

/*typedef struct movelist {
	move m;
	struct movelist *next;
} movelist;*/

typedef enum resultconst{
	WHITEWON =  100000,
	BLACKWON = -100000,
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

typedef struct bitboard{
	/* a1 = LSB
	 * a8 b8 c8 d8 ... h8
	 * .  .
	 * .	 .
	 * .		.
	 * a1 b1 c1 d1 ... h1
	 * => 
	 * h8g8...h7g7f7...h1g1f1...c1b1a1 = 0000000...0000
	 */
	
	u64 piece[12]; //as in piecenums enum
	u64 enpassanttarget;
	u64 hashValue;
		
	char castlerights; 
	/*
	* 0000 0101 => 
	* least signif 4 bit:
	*   0                1               0                0 (LSB) 
	*   black queenside, balck kingside, white queenside, white kingside
	*/
	
	//int eval;
} bitboard;

typedef enum castleRightsFlag {
	WKINGSIDE = 1,
	WQUEENSIDE = 2,
	BKINGSIDE = 4,
	BQUEENSIDE = 8
} castleRightsFlag;

typedef struct bitMove{
	uint8_t from;      // 0..63
	uint8_t to;        // 0..63
	uint8_t piece;     // 0..11
	int8_t promotion;  // 0..11, or -1
    uint8_t flags;     // bitmask
} bitMove;

typedef enum bitMoveFlags{
	CAPTURE_FLAG = 1,
	CASTLE_FLAG = 2,
	EN_PASSANT_FLAG = 4,
	PROMOTION_FLAG = 8,
	DOUBLE_PAWNMOVE_FLAG = 16
} bitMoveFlags;

typedef struct bitUndo {
	uint64_t prevHash;
	int8_t   prevEpSquareIndex;   // 0..63 or -1 = none
	int8_t   capturedPiece;       // -1 none, else 0 .. 11
	uint8_t  prevCastleRights;    // 4 bits used
	/* Last  4 bits: 0100
	 *  0                1               0                0 (LSB) 
	 *  black queenside, balck kingside, white queenside, white kingside
	 * */
	 
	//~ uint8_t  prevHalfmove;
} bitUndo;

typedef struct movearray{
	bitMove array[MAXMOVECOUNT_INPOS];
	int size;
} movearray;

typedef enum evalflag{
	EXACT_EVAL_FLAG = 1, 
	LOWER_BOUND_FLAG = 2, 
	UPPER_BOUND_FLAG = 3, 
	LAST_BEST_EVAL_FLAG = 4
}evalflag;

typedef struct TThashentry{
	u64 pos; //key
	//u64 next; 
	int eval;
	evalflag flag;
	int depth;
	//move m;
}TThashentry;

typedef struct gameInfo{
	long int startTime;
	int timeRemaining;
	bool timeControl;
	bool quit;
	int moveTime;
	
	bool newgame;
	//~ bool OwnBook;
	//~ int UCI_Elo;
	//~ bool UCI_LimitStrength;
}gameInfo;

extern const move NULLMOVE;
extern const bitMove NULLBITMOVE;

extern FILE* debugOutput;

#endif