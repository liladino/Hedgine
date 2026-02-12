#ifndef STRUCTS_H
#define STRUCTS_H

#include "consts.h"
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

typedef struct bitMove{
	uint8_t from;      // 0..63
	uint8_t to;        // 0..63
	uint8_t piece;     // 0..11
	int8_t promotion;  // 0..11, or -1
    uint8_t flags;     // bitmask
} bitMove;

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

typedef struct TThashentry{
	u64 pos; //key
	//u64 next; 
	int eval;
	evalflag flag;
	int depth; 
	bitMove m;
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