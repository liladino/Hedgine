#include "stdio.h"

/* 
 * Bonus bitmask for pawn on d5:
 * 
 * 00111000
 * 00111000
 * 00111000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 
 * for pawn on h4:
 * 
 * 00000011
 * 00000011
 * 00000011
 * 00000011
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 
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
		a = 1ULL << (i * 8);
		for (int j = 0; j < 8; j++){
			printf("%s%lld ", ((piece & a) >> (i * 8 + j) == 1 ? BG_WHITE TXT_BLACK : BG_BLACK TXT_WHITE), (piece & a) >> (i * 8 + j));
			//printf("%lld ", (piece & a) >> (i * 8 + j));
			a = a << 1;
		}
		printf(TXT_WHITE DEFAULT"\n  ");
	}
	printf("\n");
}

u64 getMaskWhitePawn(u64 pawn, int i){
	u64 mask = 0;
	
	char file = 'a' + i%8;
	int rank = 1 + i/8;

	if (rank == 1 || rank == 8) return 0;

	u64 temp = 1ULL << i;
	for (int x = rank+1; x <= 8; x++){
		temp <<= 8;
		mask |= temp;
	}

	if (file > 'a'){
		mask |= mask >> 1;
	}
	if (file < 'h'){
		mask |= mask << 1;
	}

	return mask;
}

u64 getMaskBlackPawn(u64 pawn, int i){
	u64 mask = 0;
	
	char file = 'a' + i%8;
	int rank = 1 + i/8;

	if (rank == 1 || rank == 8) return 0;

	u64 temp = 1ULL << i;
	for (int x = rank-1; x >= 1; --x){
		temp >>= 8;
		mask |= temp;
	}

	if (file > 'a'){
		mask |= mask >> 1;
	}
	if (file < 'h'){
		mask |= mask << 1;
	}

	return mask;
}

int main(){
	u64 pawn = 1;
	u64 wpawnPassedBonus[64] = {0};
	u64 bpawnPassedBonus[64] = {0};
	for (int i = 0; i < 64; i++){
		wpawnPassedBonus[i] = getMaskWhitePawn(pawn, i);
		bpawnPassedBonus[i] = getMaskBlackPawn(pawn, i);
		
		printf("%d:\n", i);
		printBitPieceAsBoard(wpawnPassedBonus[i]);
		printBitPieceAsBoard(bpawnPassedBonus[i]);
	
		char c;
		scanf("%c", &c);
		pawn = pawn << 1;
	}
	
	FILE* f = fopen("pawnPassedBonus.txt", "w");
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64; i++){
		printf("%ULLULL", wpawnPassedBonus[i]);
		fprintf(f, "%ULLULL", wpawnPassedBonus[i]);
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
		printf("%ULLULL", bpawnPassedBonus[i]);
		fprintf(f, "%ULLULL", bpawnPassedBonus[i]);
		if (i < 63){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	
	
	return 0;
}
