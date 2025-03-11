#ifndef STRUCTS_H
#define STRUCTS_H

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

/* This is a REALLY rough estimate, an upper limit calculated with an 
 * impossible position.
 * 
 * A statistical analysis by others shows, that on average, a game has at most 
 * 40 legal moves in one move.
 * https://chess.stackexchange.com/questions/23135/what-is-the-average-number-of-legal-moves-per-turn
 * 
 * I should make some statistical tests about the maximal number of legal moves 
 * in each position, to make a better estimate, as i suspect that it could 
 * improve performance. by a significant amount.
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

#define REPETITION_TABLE_SIZE 1024

typedef unsigned long long u64;

#include <stdbool.h>
#include <stdio.h>

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

typedef struct movelist {
	move m;
	struct movelist *next;
} movelist;

typedef enum resultconst{
	whitewon =  100000,
	blackwon = -100000,
	draw = 0,
	ongoing = -1
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
		
	char castlerights; 
	/*
	* 0000 0101 => 
	* least signif 4 bit:
	*               0               1                0               1 (LSB) 
	* black queenside, balck kingside, white queenside, white kingside
	*/
	
	u64 enpassanttarget;
	u64 hashValue;
	//int eval;
}bitboard;

typedef struct movearray{
	bitboard boards[MAXMOVECOUNT_INPOS];
	int size;
} movearray;

typedef enum evalflag{
	exactFlag = 1, alphaFlag = 2, betaFlag = 3, lastBest = 4
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

extern const move nullmove;

extern FILE* debugOutput;

#endif
