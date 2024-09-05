#include "main.h"


int main(){
	#ifdef DEBUG
	long int runTime = getTime_ms();
	#endif
	
	initializeAll();
	
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	
	bitboard board;

	//main uci loop
	UCIloop(&board, &tomove, &fmv, &movenum);
			
	freeTransTable();
	
	#ifdef DEBUG
	printf("runtime: %ld\n", getTime_ms() - runTime);
	#endif
	
	return 0;
}
	
void moveReaderTest(){
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	
	bitboard board;
	setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &tomove, &fmv, &movenum);
	
	char movestr[5] = {0};

	printBitBoard2d(board);
	while (scanf("%5s", movestr) != EOF){
		move m = parseLongAlgebraicNotation(movestr);
		
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
	
//~ void LANTester(){
	//~ printf("Long algebraic notation test\n----\nGive a move (e.g. e2e4):\n");
	//~ char movestr[5] = {0};
	//~ scanf("%5s", movestr);

	//~ move m = parseLongAlgebraicNotation(movestr);
	//~ printmove(m);
//~ }

//~ void getLineDynamictester(){
	//~ size_t limit = 10;
	//~ printf("\n\nGetline test\n----\nGive strings (stops at %ld characters):\n", limit);
	//~ char* str = NULL;
	//~ int a;
	//~ while((a = getLineDynamic(&str, limit))){
		//~ printf("%s,\t%d\n", str, a);
	//~ }
	//~ printf("%s,\t%d\n", str, a);
	//~ free(str);
//~ }
