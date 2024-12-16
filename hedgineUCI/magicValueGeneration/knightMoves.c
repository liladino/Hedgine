#include "stdio.h"

/* This program is only used to generate the array of bitmasks where a knight 
 * can move from each of the 64 squares. 
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

void getmask(u64 knight, int i, u64* write){
	char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (rank >= 2 && file >= 'c'){
		*write = (knight >> 10);
	}
	write++;
	if (rank <= 7 && file >= 'c'){
		*write = (knight << 6);
	}
	write++;
	
	if (rank >= 2 && file <= 'f'){
		*write = (knight >> 6);
	}
	write++;
	if (rank <= 7 && file <= 'f'){
		*write = (knight << 10);
	}
	write++;
	
	if (file >= 'b' && rank >= 3){
		*write = (knight >> (sout + soWe));
	}
	write++;
	if (file <= 'g' && rank >= 3){
		*write = (knight >> (sout + soEa));
	}
	write++;

	if (file >= 'b' && rank <= 6){
		*write = (knight << (nort + noWe));
	}
	write++;
	if (file <= 'g' && rank <= 6){
		*write = (knight << (nort + noEa));
	}
	write++;
}

int main(){
	u64 knight = 1;
	u64 kinghtMoves[64 * 8] = {0};
	for (int i = 0; i < 64; i++){
		getmask(knight, i, kinghtMoves + i*8);
	
		printf("%d:\n", i);
		printBitPieceAsBoard(kinghtMoves[i*8] | kinghtMoves[i*8 + 1] | kinghtMoves[i*8 + 2] | kinghtMoves[i*8 + 3] | kinghtMoves[i*8 + 4] | kinghtMoves[i*8 + 5] | kinghtMoves[i*8 + 6] | kinghtMoves[i*8 + 7]);
	
		char c;
		scanf("%c", &c);
	
		knight = knight << 1;
	}
	
	FILE* f = fopen("kinghtMoves.txt", "w");
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64 * 8; i++){
		printf("%lluLLU", kinghtMoves[i]);
		fprintf(f, "%lluLLU", kinghtMoves[i]);
		if (i < 64 * 8 - 1){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	return 0;
}
