#include "main.h"


int main(){
	long int startTime = getTimems();
	
	initializeAll();
	
	//main uci loop
	//~ UCIloop();
	
	//~ moveReaderTest();
	
	
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	int castling[4];
	squarenums enpass = {-1, -1};
	
	bitboard board;
	setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &tomove, castling, &enpass, &fmv, &movenum);
	
	printBitBoard2d(board);
	
	info.timeControl = false;
	info.moveTime = 10000;
	CPU(4, board, white);
	
	freeTransTable();
	
	printf("runtime: %ld\n", getTimems() - startTime);
	
	return 0;
}
	
void moveReaderTest(){
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	int castling[4];
	squarenums enpass = {-1, -1};
	
	bitboard board;
	setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &tomove, castling, &enpass, &fmv, &movenum);
	
	char movestr[5] = {0};

	printBitBoard2d(board);
	while (scanf("%5s", movestr) != EOF){
		move m = readLongAlgebraicNotation(movestr);
		
		int x = isMoveLegal(&board, tomove, m);
		if (x == 1){
			continue;
		}
		
		if (tomove == black) movenum++;
		tomove = !tomove;
		//update fmv
		//..or maybe not, it unnecesarily complicates matters
		
		printBitBoard2d(board);
		
		if (x == 2) break;
	}
}
	
void LANTester(){
	printf("Long algebraic notation test\n----\nGive a move (e.g. e2e4):\n");
	char movestr[5] = {0};
	scanf("%5s", movestr);

	move m = readLongAlgebraicNotation(movestr);
	printmove(m);
}

void getLinetester(){
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
