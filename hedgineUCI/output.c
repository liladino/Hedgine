#include "output.h"

void printmove(FILE* ostream, move m) {
	fprintf(ostream, "%c%d%c%d%c ", m.from.file+'a'-2, m.from.rank-1, m.to.file+'a'-2, m.to.rank-1, (m.promotion >= 'a' && m.promotion <= 'z' ? m.promotion : ' ') );
}

#ifdef DEBUG
void printLegalmoves(movearray legalmoves, bitboard board, bool tomove){
	printf("\nLegal moves:\n");
	for (int i = 0; i < legalmoves.size; i++){
		printmove(stdout, boardConvertTomove(&board, &legalmoves.boards[i], tomove));
		printHashEntry(legalmoves.boards[i].hashValue);
		//~ printf("%lf\n", legalmoves.boards[i].eval * 0.01);
	}
}
#endif


/* prints the board to a specified output stremm with characters. */
void printboardLetters(FILE* ostream, char board[12][12]){
	fprintf(ostream, "|");
	for (int j = 2; j < 9; j++){
		fprintf(ostream, "---+");
	}
		fprintf(ostream, "---|\n");
	for (int i = 9; i > 1; i--){
		fprintf(ostream, "|");
		for (int j = 2; j < 10; j++){
			fprintf(ostream, " %c |", board[i][j]);
		}
		fprintf(ostream, "\n|");
		for (int j = 2; j < 9; j++){
			fprintf(ostream, "---+");
		}
		fprintf(ostream, "---|\n");
	}
	fprintf(ostream, "\n");
}

void printBitBoard2d(FILE* ostream, bitboard bboard){
	char board[12][12];
	boardConvertBack(board, bboard);
	printboardLetters(ostream, board);
}

void printBitBoard(bitboard board){
	printf("ranks    8.       7.       6.       5.       4.       3.       2.       1.\nwhite:   \n pawn   ");
	printBitPiece(board.piece[wpawn]);

	printf(" knight ");
	printBitPiece(board.piece[wknight]);
	
	printf(" bishop ");
	printBitPiece(board.piece[wbishop]);
	
	printf(" rook   ");
	printBitPiece(board.piece[wrook]);
	
	printf(" queen  ");
	printBitPiece(board.piece[wqueen]);
	
	printf(" king   ");
	printBitPiece(board.piece[wking]);
	
	printf("\nblack:\n pawn   ");
	printBitPiece(board.piece[bpawn]);

	printf(" knight ");
	printBitPiece(board.piece[bknight]);
	
	printf(" bishop ");
	printBitPiece(board.piece[bbishop]);
	
	printf(" rook   ");
	printBitPiece(board.piece[brook]);
	
	printf(" queen  ");
	printBitPiece(board.piece[bqueen]);
	
	printf(" king   ");
	printBitPiece(board.piece[bking]);
	printf("\n");
}

void printBitPiece(u64 piece){
	u64 a = 1;
	a = a << 63;
	for (int i = 63; i >= 0; i--){
		printf("%lld", (piece & a) >> i);
		a = a >> 1;
		if (i % 8 == 0) printf(" ");
	}
	printf("\n");
}

void printBitPieceAsBoard(u64 piece){
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


