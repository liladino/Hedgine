#include "stdio.h"

/* This program is only used to generate the array of bitmasks where a pawn 
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

u64 getMaskBlackPawn(u64 pawn, int i){
	u64 mask = 0;
	
	// char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (1 < rank && rank < 8){
		mask |= pawn >> sout;
        if (7 == rank){
            mask |= pawn >> 2*sout;
        }
	}
	return mask;
}

u64 getMaskWhitePawn(u64 pawn, int i){
	u64 mask = 0;
	
	// char file = 'a' + i%8;
	int rank = 1 + i/8;
	
	if (1 < rank && rank < 8){
		mask |= pawn << nort;
        if (2 == rank){
            mask |= pawn << 2*nort;    
        }
	}
	return mask;
}

int main(){
	u64 pawn = 1;
	u64 wpawnMoves[64] = {0};
	u64 bpawnMoves[64] = {0};
	for (int i = 0; i < 64; i++){
		wpawnMoves[i] = getMaskWhitePawn(pawn, i);
		bpawnMoves[i] = getMaskBlackPawn(pawn, i);
		
		printf("%d:\n", i);
		printBitPieceAsBoard(wpawnMoves[i]);
		printBitPieceAsBoard(bpawnMoves[i]);
	
		// char c;
		// scanf("%c", &c);
		pawn = pawn << 1;
	}
	
	FILE* f = fopen("pawnMoves.txt", "w");
	printf("{");
	fprintf(f, "{");
	for (int i = 0; i < 64; i++){
		printf("%lluLLU", wpawnMoves[i]);
		fprintf(f, "%lluLLU", wpawnMoves[i]);
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
		printf("%lluLLU", bpawnMoves[i]);
		fprintf(f, "%lluLLU", bpawnMoves[i]);
		if (i < 63){
			printf(", ");
			fprintf(f, ", ");
		}
	}
	printf("}\n");
	fprintf(f, "}\n");
	
	
	
	return 0;
}
