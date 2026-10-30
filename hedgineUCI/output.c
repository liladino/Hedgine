#include "headers/output.h"

/* 
 * my print "wrapper". 
 * usage: printDebug("meow meow %d", 10);
 * result:
 *  printf("meow meow %d", 10);
 *  fprintf(g_debugOutput, "meow meow %d", 10);
 * 
 * stolen code.
 * */
void printDebug(
#ifdef DEBUG
	const char* log, ...
#endif
	) {
#ifdef DEBUG
	va_list args1, args2;
	va_start(args1, log);
	
	// Copy args for the second call
	va_copy(args2, args1);

	vprintf(log, args1); 
	vfprintf(g_debugOutput, log, args2); 

	va_end(args1);
	va_end(args2);
#endif
}

void printmove(FILE* ostream, move m) {
	if (m.from.file == NULLMOVE.from.file){
		//UCI null move
		fprintf(ostream, "0000 ");
		return;
	}
	fprintf(ostream, "%c%d%c%d%c ", m.from.file+'a'-2, m.from.rank-1, m.to.file+'a'-2, m.to.rank-1, (m.promotion >= 'a' && m.promotion <= 'z' ? m.promotion : ' ') );
}

void printBitMove(FILE* ostream, bitMove m) {
    static const char* pieceNames[] = {
        "wking", "wqueen", "wrook", "wbishop", "wknight", "wpawn",
        "bking", "bqueen", "brook", "bbishop", "bknight", "bpawn"
    };

    // Convert square indices (0..63) to algebraic notation (a1..h8).
    // Assumes a1 = 0, h1 = 7, a8 = 56, h8 = 63.
    char from[3] = {'a' + (m.from % 8), '1' + (m.from / 8), '\0'};
    char to[3]   = {'a' + (m.to % 8),   '1' + (m.to / 8),   '\0'};

    fprintf(ostream, "piece %s, from %s, to %s, prom %s, flags ",
        m.piece < 12 ? pieceNames[m.piece] : "invalid",
        from,
        to,
        (m.promotion >= 0 && m.promotion < 12)
            ? pieceNames[m.promotion]
            : "none"
    );

    if (m.flags == 0) {
        fprintf(ostream, "NONE");
    }
    else {
        bool first = true;

        #define PRINT_FLAG(flag, name)             \
            if (m.flags & (flag)) {                \
                if (!first) fprintf(ostream, "|"); \
                fprintf(ostream, "%s", (name));    \
                first = false;                     \
            }

        PRINT_FLAG(CAPTURE_FLAG, "CAPTURE");
        PRINT_FLAG(CASTLE_FLAG, "CASTLE");
        PRINT_FLAG(EN_PASSANT_FLAG, "EN_PASSANT");
        PRINT_FLAG(PROMOTION_FLAG, "PROMOTION");
        PRINT_FLAG(DOUBLE_PAWNMOVE_FLAG, "DOUBLE_PAWNMOVE");

        #undef PRINT_FLAG
    }

    fprintf(ostream, " ");
}

#ifdef DEBUG
void printLegalmoves(movearray legalmoves, bitboard board){
	printf("\nLegal moves:\n");
	for (int i = 0; i < legalmoves.size; i++){
		printBitMove(stdout, legalmoves.array[i]);
		printf("\n");
		bitUndo u = makeMove(&board, &legalmoves.array[i]);
		// printHashEntry(board.hashValue);
		undoMove(&board, &legalmoves.array[i], &u);
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

void printBitsOfNumber(u64 number, int numberOfBits){
	u64 a = 1;
	a = a << (numberOfBits-1);
	for (int i = numberOfBits-1; i >= 0; i--){
		printf("%lld", (number & a) >> i);
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


