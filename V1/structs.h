#ifndef STRUCTS_H
#define STRUCTS_H

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
#define TXT_WHITE "\x1b[37m"
#define TXT_BOLD "\x1B[1m"
#define DEFAULT "\x1b[37m" "\x1b[m"
#define CLEAR "\033[2J\033[H"

typedef struct square{
	char file; //vonal
	int rank; //sor
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

typedef struct movewitheval {
	move m;
	double eval;
} movewitheval;

typedef enum resultconst{
	whitewon = 1024,
	blackwon = -1024,
	draw = 0,
	stalemate = 2048
}result;

#endif
