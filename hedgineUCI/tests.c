#include "tests.h"

void printLogo(){
	printf("                                     /    #\n .  .     .                       #   *.  # #   ##  .#\n |__| _  _| _ . _  _       # (  #       #.   #,     /,##,     #(\n |  |(/,(_](_]|[ )(/,  (  #   ##                             # .,\n           ._|        # #.,                                      #\n                     #   #                                              /#\n                  ./##                           ||                  *#\n                   #                            ====                     ##\n                   #                       ____  ||  ____                  #\n                  +.    &&                / __ \\ /\\ / __ \\                /#\n                 &   &&     &&           | /  \\ |  | /  \\ |               #\n     &&*       &    *&& #      &         | \\   \\ \\/ /   / |                ,.\n   &    &#+&&     .#    &       &         \\ \\__/ || \\__/ /                (,\n   &     &         &  & &        &        |______________|                #*#\n    &&&&.           /&&          &         \\____________/                #\n        &                       /.                                 (###/  #\n         &        # &           &                                   (\n           *&      &           &                        .##    #   ###\n               *&&          *&#  ,##   #    ((##( /   ##  #  #*#\n                                     (#    ###     \n");	
}


int perfTest(bitboard board, bool tomove, int depth){	
	movearray legalmoves;
	
	bitGenerateLegalmoves(&legalmoves, board, tomove, false);
	if (depth == 0){
		return 1;
	}
	int all = 0;
	for (int i = 0; i < legalmoves.size; i++){
		int x = perfTest(legalmoves.boards[i], !tomove, depth-1);
		all += x;
	}
	return all;
}

/*
 * ASSERTION TEST
 * */
bool makePerfTestsAssert(){
	//Source: https://www.chessprogramming.org/Perft_Results
	
	bitboard bboard;
	int temp;
	bool tomove;
	printf("\nPosition 1\n");
	setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int startposValues[] = {1, 20, 400, 8902, 197281};
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, startposValues[i]);
		if (allmovecount != startposValues[i]) return false;
	}
	
	//skip the test for now
	goto test3;
	
	printf("\nPosition 2\n");
	setboardFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int pos2Values[] = {1, 48, 2039, 97862, 4085603};
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos2Values[i]);
		if (allmovecount != pos2Values[i]) return false;
	}
	
	test3:
	printf("\nPosition 3\n");
	setboardFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int pos3Values[] = {1, 14, 191, 2812, 43238};
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos3Values[i]);
		if (allmovecount != pos3Values[i]) return false;
	}
	
	printf("\nPosition 4\n");
	setboardFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int pos4Values[] = {1, 6, 264, 9467, 422333};
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos4Values[i]);
		if (allmovecount != pos4Values[i]) return false;
	}
	
	printf("\nPosition 4b\n");
	setboardFEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos4Values[i]);
		if (allmovecount != pos4Values[i]) return false;
	}
	
	printf("\nPosition 5\n");
	setboardFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int pos5Values[] = {1, 44, 1486, 62379, 2103487};
	for (int i = 0; i < 5; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos5Values[i]);
		if (allmovecount != pos5Values[i]) return false;
	}
	
	printf("\nPosition 6\n");
	setboardFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", &bboard, &tomove, &temp, &temp);
	printBitBoard2d(stdout, bboard);
	int pos6Values[] = {1, 46, 2079, 89890};
	for (int i = 0; i < 4; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", i+1, allmovecount, pos6Values[i]);
		if (allmovecount != pos6Values[i]) return false;
	}
	
	return true;
}


#define benchdepth 4

/*
 * BENCHMARK
 * */
void makePerfTest(char board[12][12], bool tomove, int castling[4], squarenums enpass){
	printLogo();
	bitboard bboard = boardConvert(board, castling, enpass, tomove);
	for (int i = 0; i < benchdepth; i++){
		int allmovecount = perfTest(bboard, tomove, i);
		printf("%d ply:\nLeaves: %d\n\n", i+1, allmovecount);
	}
}
