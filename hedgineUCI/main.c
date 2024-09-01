#include "main.h"


int main(){
	char board2d[12][12] = {0};
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	int castling[4];
	squarenums enpass = {-1, -1};
	
	//~ setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board2d, &tomove, castling, &enpass, &fmv, &movenum);
	setboardFEN("rnbqkbnr/1pppppPp/8/8/8/8/PpPPPPP1/RNBQKBNR w KQkq - 0 5", board2d, &tomove, castling, &enpass, &fmv, &movenum);
	bitboard board = boardConvert(board2d, castling, enpass, tomove);
	char movestr[5] = {0};
	
	movearray legalmoves;
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	
	printBitBoard2d(board);
	while (scanf("%5s", movestr) != EOF){
		move m = readLongAlgebraicNotation(movestr);
		
		int x = isMoveLegal(&board, &legalmoves, tomove, m);
		if (x == -1){
			continue;
		}
		
		board = legalmoves.boards[x];
		
		if (tomove == black) movenum++;
		tomove = !tomove;
		//update fmv
		//..or maybe not, it unnecesarily complicates matters
		
		printBitBoard2d(board);
		
		
		legalmoves.size = 0;
		bitGenerateLegalmoves(&legalmoves, board, tomove, false);
		if (legalmoves.size == 0) break;
	}
	
	
	
	return 0;
}
	
	
	
	
	
void tester(){
	printf("Long algebraic notation test\n----\nGive a move (e.g. e2e4):\n");
	char movestr[5] = {0};
	scanf("%5s", movestr);

	move m = readLongAlgebraicNotation(movestr);
	printmove(m);

	size_t limit = 10;
	printf("\n\nGetline test\n----\nGive strings (stops at %ld characters):\n", limit);
	char* str = NULL;
	int a;
	while((a = getLine(&str, limit))){
		printf("%s,\t%d\n", str, a);
	}
	printf("%s,\t%d\n", str, a);
	free(str);
}
