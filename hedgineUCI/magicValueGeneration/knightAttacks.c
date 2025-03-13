#include "stdio.h"

/* This program is only used to generate the array of bitmasks where a knight 
 * can move from each of the 64 squares. The square's index can be used to get 
 * the knight moves from there, helping the check detection (and possibly a 
 * single instruction check to see if the current knight attacks any enemy 
 * piece).
 * */

typedef unsigned long long u64;
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

void printBitPieceAsBoard(u64 piece){
	#define BG_BLACK "\x1b[40m"
	#define BG_WHITE "\x1b[47m"
	#define TXT_BLACK "\x1b[30m"
	#define TXT_WHITE "\x1b[37m"
	#define DEFAULT "\x1b[37m" "\x1b[m"
	u64 a = 1;
	printf("  ");
	for (int i = 7; i >= 0; i--){
		a = 1LL << (i * 8);
		for (int j = 0; j < 8; j++){
			printf("%s%lld ", ((piece & a) >> (i * 8 + j) == 1 ? BG_WHITE TXT_BLACK : BG_BLACK TXT_WHITE), (piece & a) >> (i * 8 + j));
			//printf("%lld ", (piece & a) >> (i * 8 + j));
			a = a << 1;
		}
		printf(TXT_WHITE DEFAULT"\n  ");
	}
	printf("\n");
}

u64 getmask(u64 knight, int i){
	u64 mask = 0;
	
	char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (file >= 'c'){
		if (rank >= 2){
			mask = mask | (knight >> 10);
		}
		if (rank <= 7){
			mask = mask | (knight << 6);
		}
	}
	if (file <= 'f'){
		if (rank >= 2){
			mask = mask | (knight >> 6);
		}
		if (rank <= 7){
			mask = mask | (knight << 10);
		}
	}
	
	if (rank >= 3){
		if (file >= 'b'){
			mask = mask | (knight >> (sout + soWe));
		}
		if (file <= 'g'){
			mask = mask | (knight >> (sout + soEa));
		}
	}
	if (rank <= 6){
		if (file >= 'b'){
			mask = mask | (knight << (nort + noWe));
		}
		if (file <= 'g'){
			mask = mask | (knight << (nort + noEa));
		}
	}
	
	return mask;
}

int main(){
	u64 knight = 1;
	u64 knightAttacks[64] = {0};
	for (int i = 0; i < 64; i++){
		knightAttacks[i] = getmask(knight, i);
	
		printf("%d:\n", i);
		printBitPieceAsBoard(knightAttacks[i]);
	
		knight = knight << 1;
	}
	
	FILE* f = fopen("knightAttacks.txt", "w");
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64; i++){
		printf("%lluLLU", knightAttacks[i]);
		fprintf(f, "%lluLLU", knightAttacks[i]);
		if (i < 63){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	return 0;
}
