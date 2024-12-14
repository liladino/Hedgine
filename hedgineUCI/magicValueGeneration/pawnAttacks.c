#include "stdio.h"

/* This program is only used to generate the array of bitmasks where a pawn 
 * can take from each of the 64 squares. This is used when looking for pawn 
 * checks.
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

u64 getMaskBlackKing(u64 king, int i){
	u64 mask = 0;
	
	char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (rank > 1){
		if (file > 'a'){
			mask |= king >> soWe;
		}
		if (file < 'h'){
			mask |= king >> soEa;
		}
	}	
	return mask;
}

u64 getMaskWhiteKing(u64 king, int i){
	u64 mask = 0;
	
	char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (rank < 8){
		if (file > 'a'){
			mask |= king << noWe;
		}
		if (file < 'h'){
			mask |= king << noEa;
		}
	}	
	return mask;
}

int main(){
	u64 king = 1;
	u64 wpawnAttacks[64] = {0};
	u64 bpawnAttacks[64] = {0};
	for (int i = 0; i < 64; i++){
		wpawnAttacks[i] = getMaskWhiteKing(king, i);
		bpawnAttacks[i] = getMaskBlackKing(king, i);
		
		printf("%d:\n", i);
		printBitPieceAsBoard(wpawnAttacks[i]);
		printBitPieceAsBoard(bpawnAttacks[i]);
	
		//~ char c;
		//~ scanf("%c", &c);
		king = king << 1;
	}
	
	FILE* f = fopen("pawnAttacks.txt", "w");
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64; i++){
		printf("%lluLLU", wpawnAttacks[i]);
		fprintf(f, "%lluLLU", wpawnAttacks[i]);
		if (i < 63){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64; i++){
		printf("%lluLLU", bpawnAttacks[i]);
		fprintf(f, "%lluLLU", bpawnAttacks[i]);
		if (i < 63){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	
	
	return 0;
}
