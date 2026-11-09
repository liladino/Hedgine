#include "headers/tests.h"

void printLogo(){
	printf("                                     /    #\n .  .     .                       #   *.  # #   ##  .#\n |__| _  _| _ . _  _       # (  #       #.   #,     /,##,     #(\n |  |(/,(_](_]|[ )(/,  (  #   ##                             # .,\n           ._|        # #.,                                      #\n                     #   #                                              /#\n                  ./##                           ||                  *#\n                   #                            ====                     ##\n                   #                       ____  ||  ____                  #\n                  +.    &&                / __ \\ /\\ / __ \\                /#\n                 &   &&     &&           | /  \\ |  | /  \\ |               #\n     &&*       &    *&& #      &         | \\   \\ \\/ /   / |                ,.\n   &    &#+&&     .#    &       &         \\ \\__/ || \\__/ /                (,\n   &     &         &  & &        &        |______________|                #*#\n    &&&&.           /&&          &         \\____________/                #\n        &                       /.                                 (###/  #\n         &        # &           &                                   (\n           *&      &           &                        .##    #   ###\n               *&&          *&#  ,##   #    ((##( /   ##  #  #*#\n                                     (#    ###     \n");	
}


void testLasker(){
	initializeAll();
	
	printf("Lasker-Reichhelm position: 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1\n");
	bitboard board;
	int temp;
	bool tomove;
	
	char* command = calloc(100, sizeof(char));
	strcpy(command, "position fen 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1");
	parsePosition(command, &board, &tomove, &temp, &temp);
	printBitBoard2d(stdout, board);
	
	strcpy(command, "go movetime 15000");
	parseGo(command, &board, &tomove);	
	
	free(command);
}

void testEnglish(){
	initializeAll();
	
	printf("Anglo-Indian defense: r3k2r/pp1bppbp/2n4q/2p5/2P1B3/2B2P2/PPQ3PP/1K1R2NR b kq - 2 14\n");
	bitboard board;
	int temp;
	bool tomove;
	
	char* command = calloc(100, sizeof(char));
	strcpy(command, "position fen r3k2r/pp1bppbp/2n4q/2p5/2P1B3/2B2P2/PPQ3PP/1K1R2NR b kq - 2 14");
	parsePosition(command, &board, &tomove, &temp, &temp);
	printBitBoard2d(stdout, board);
	
	strcpy(command, "go movetime 15000");
	parseGo(command, &board, &tomove);
	
	free(command);
}


bool bitBoardCompare(const bitboard* const b1, const bitboard* const b2){
	for (int i = 0; i < 12; i++){
		if (b1->piece[i] != b2->piece[i]) { return false; }
	}
	if (b1->enpassanttarget != b2->enpassanttarget) { return false; }
	if (b1->hashValue != b2->hashValue)             { return false; }
	if (b1->castlerights != b2->castlerights)       { return false; }
	return true;
}

bool makeMoveTest(){
	// return true;
	bitboard board;
	bool tomove;
	int temp;
	setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &board, &tomove, &temp, &temp);
	printBitBoard2d(stdout, board);
	board.hashValue = hashPosition(&board, tomove);
	
	char* moves[] = {"h2h4", "d7d5", "h4h5", "c8g4", "h5h6", "b8c6", "h6g7", "d8d6", "g7h8n", "e8c8", "h1h2", "d5d4", "e2e4", "d4e3", "f2e3", "g4d1", "e1f2",
		"h7h5", "h2h3", "h5h4", "h3f3", "h4h3", "f3f4", "h3h2", "f4f3", "h2h1r"}; //26
	setMailBox(&board);
	
	//~ printMailBox();
		
	for (size_t i = 0; i < 26; i++){
		printf("%s\n", moves[i]);
				
		bitboard copy = board;
		bitMove m = convertMoveToBitMove(&copy, tomove, parseLongAlgebraicNotation(moves[i]));
		bitUndo u = makeMove(&copy, &m);
		
		if (hashPosition(&copy, !tomove) != copy.hashValue){
			goto hashfail;
		}
		
		undoMove(&copy, &m, &u);
		if (!bitBoardCompare(&board, &copy)){
			printBitBoard2d(stdout, board);
			printBitBoard2d(stdout, copy);
			goto fail;
		}
		
		//make the move actually
		makeMove(&board, &m);
		tomove = !tomove;
		
		//~ printBitsOfNumber((u64)board.castlerights, 8);
	}
	printBitBoard2d(stdout, board);
	printBitPieceAsBoard(board.enpassanttarget);
	printBitsOfNumber((u64)board.castlerights, 8);
	printMailBox();
	
	printf("makeMove & undoMove tests passed\n");
	return true; 
	
	fail:
	printf("makeMove & undoMove tests failed\n");
	return false;
	
	hashfail:
	printf("makeMove & undoMove tests failed: hashing\n");
	return false;
}

int perfTest(bitboard* board, bool tomove, int depth){	
	movearray legalmoves;
	#ifdef DEBUG
	if (hashPosition(board, tomove) != board->hashValue){
		printf("Hashing failed!");
		return 0;
	}
	#endif
	
	if (depth == 0){
		return 1;
	}
	
	//old, pure move generation logic, so that ordering, etc. doesn't slow it
	generatePseudoLegalMoves(&legalmoves, board, tomove);
	// printLegalmoves(legalmoves, *board);
	int all = 0;
	for (int i = 0; i < legalmoves.size; i++){
		if (!isCastlingLegal(board, tomove, &legalmoves.array[i])){
			continue;
		}
		
		//~ bitMove temp = legalmoves.array[i];
		bitUndo u = makeMove(board, &legalmoves.array[i]);
		
		if (bitInCheck(board, tomove)){
			undoMove(board, &legalmoves.array[i], &u);
			//~ printBitBoard2d(stdout, *board);
			continue;
		}
		
		int x = perfTest(board, !tomove, depth-1);
		undoMove(board, &legalmoves.array[i], &u);
		all += x;
	}
	
	return all;
}

/*
 * TEST POSITIONS
 * Source: https://www.chessprogramming.org/Perft_Results and stockfish
 *
 * leaves[d] is the expected perft result at depth d. The assertion test
 * checks the depths in [minDepth, maxDepth].
 * */
typedef struct perftCase {
	const char* name;
	const char* fen;
	int minDepth;
	int maxDepth;
	int leaves[9];
} perftCase;

static const perftCase PERFT_CASES[] = {
	{"Position 1", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 0, 4, {1, 20, 400, 8902, 197281, 4865609, 119060324}},
	{"Position 2", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 0, 4, {1, 48, 2039, 97862, 4085603}},
	{"Position 3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 0, 4, {1, 14, 191, 2812, 43238}},
	{"Position 4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 0, 4, {1, 6, 264, 9467, 422333}},
	{"Position 4b", "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 0, 4, {1, 6, 264, 9467, 422333}},
	{"Position 5", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 0, 4, {1, 44, 1486, 62379, 2103487}},
	{"Position 6", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 0, 3, {1, 46, 2079, 89890}},
	{"Position 7", "8/2p5/3p4/1P5r/1K5k/5p2/4P1P1/1R6 b - - 0 1", 0, 4, {1, 18, 350, 6330, 120269}},
	{"Position 8", "r3k2r/p6p/4N3/1P4P1/1p4p1/4n3/P6P/R3K2R w KQkq - 0 1", 2, 4, {1, 21, 427, 9601, 221941}},
	{"Position 9", "4k3/P7/8/8/8/8/8/QQQQKQQQ w - - 0 1", 0, 5, {1, 102, 106, 11094, 19905, 2104182}},
	{"Position 10", "4k3/P7/8/8/8/8/QQQQ1QQQ/RNBQKBNR w - - 0 1", 0, 6, {1, 107, 80, 8965, 6850, 780087, 688687}},
	{"Position 11 (Lasker)", "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1", 0, 8, {1, 3, 15, 90, 396, 2090, 10545, 61641, 301431}}

	//middlegame positions for the search speed test, no perft results (maxDepth < minDepth: nothing is asserted)
	// , 
	// {"Middlegame 1", "r2q1rk1/pp2ppbp/2n3p1/2p5/2BPP1b1/2P1B3/P3NPPP/R2Q1RK1 w - - 8 11", 0, -1, {0}},
	// {"Middlegame 2", "r1bqkb1r/5ppp/p2p4/1P1np3/3nP3/N7/PP3PPP/R1BQKB1R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 3", "rn1q1rk1/ppb2ppp/2ppbn2/4p3/1PPP4/P1N1P1P1/4NPBP/R1BQ1RK1 w - - 2 11", 0, -1, {0}},
	// {"Middlegame 4", "2rqk2r/1p1bppbp/p1np1np1/8/3NP3/1BN1BP2/PPPQ2PP/R3K2R w KQk - 0 11", 0, -1, {0}},
	// {"Middlegame 5", "r2qkb1r/ppp2p1p/2npb1pB/1N1Qp3/8/1B1P4/PPP2PPP/R3K1NR w KQkq - 1 11", 0, -1, {0}},
	// {"Middlegame 6", "r1br2k1/ppp2pbp/2n3p1/3nN3/5B2/4P2P/PPP1BPP1/RN3RK1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 7", "r1b1kb1r/3n1pp1/p3pq1p/1pp5/3PP3/3B1N2/PPQ2PPP/RN2K2R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 8", "r2q1rk1/1p2ppbp/p1np1np1/2p5/2P1PP2/2NPBb1P/PP2B1P1/R2Q1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 9", "r2qnrk1/pb1nbppp/1pp1p3/3pP3/2PP4/5NP1/PPQN1PBP/R1B2RK1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 10", "r1b2rk1/ppq1ppbp/2n3p1/1B1pP3/3Pn3/5N1P/PP3PP1/RNBQR1K1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 11", "r1bqk2r/5ppp/p1np1b2/1p1Np3/4P3/N7/PPP2PPP/R2QKB1R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 12", "1r1q1rk1/pp2ppbp/2np1np1/2p5/4PP2/2NP1QPP/PPP3B1/R1B2RK1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 13", "2rq1rk1/pp1bppbp/2np1np1/8/2BNP1P1/2N1BP2/PPPQ3P/R3K2R w KQ - 1 11", 0, -1, {0}},
	// {"Middlegame 14", "r1bqnrk1/ppp1n1bp/3p2p1/3Ppp2/1PP1P3/2N5/P2NBPPP/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 15", "r1bq1rk1/pp2ppbp/1nnp2p1/4P3/3P4/1BN2N2/PP3PPP/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 16", "r1b1k2r/pp1nbppp/1q2p3/n2pP3/3P1P1P/P4N2/1P4P1/R1BQKBNR w KQkq - 1 11", 0, -1, {0}},
	// {"Middlegame 17", "rn1qkb1r/pp3pp1/2p1pn1p/4N3/3P3P/3Q2N1/PPP2PP1/R1B1K2R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 18", "r1b2rk1/1pq1ppbp/p1np1np1/8/2PN4/1PN3P1/PB2PPBP/R2Q1RK1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 19", "r1bk3r/ppp2ppp/2p5/8/2B5/2b5/P1P2PPP/R1B2RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 20", "2rqk2r/1p1bppbp/p1np1np1/8/3NP1P1/2N1BP2/PPPQ3P/2KR1B1R w k - 0 11", 0, -1, {0}},
	// {"Middlegame 21", "r2q1rk1/pp1nbppp/3p1n2/2pP4/2P3b1/2NB1N2/PP3PPP/R1BQ1RK1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 22", "r2q1rk1/pp1nbpp1/2p1p2p/3p1b2/2PPn3/1P3NP1/PB1NPPBP/R2QR1K1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 23", "r3k2r/pp1nbppp/2p1pn2/q7/3P1B2/2N2B1P/PPP2PP1/R2Q1RK1 w kq - 1 11", 0, -1, {0}},
	// {"Middlegame 24", "r1bq1rk1/4bppp/p2p1n2/npp1p3/4P3/2PP1N1P/PPB2PP1/RNBQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 25", "r1b2rk1/pp1nqppp/2pb1n2/4p3/2PP4/2N1PN2/PBQ1BPPP/R3K2R w KQ - 2 11", 0, -1, {0}},
	// {"Middlegame 26", "rn1q1rk1/1b2bppp/p2ppn2/1p6/3NPP2/PBN5/1PP3PP/R1BQ1RK1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 27", "rnb2rk1/pp2qpp1/2p1p2p/3p4/2PP4/2R1PN2/PP3PPP/3QKB1R w K - 0 11", 0, -1, {0}},
	// {"Middlegame 28", "r1bq1rk1/pp2ppbp/2n3p1/1B1pP3/3P4/2n2N2/PP3PPP/R1BQR1K1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 29", "r2q1rk1/pp3ppp/2n1bn2/2bp2B1/8/2N2NP1/PP2PPBP/R2Q1RK1 w - - 4 11", 0, -1, {0}},
	// {"Middlegame 30", "r1b1k2r/1pp3pp/p1pb1pn1/8/3NP3/2N1B3/PPP2PPP/2KR3R w kq - 4 11", 0, -1, {0}},
	// {"Middlegame 31", "r1b1kb1r/pp1n1ppp/4p3/3pP3/3q4/3B4/PP1N1PPP/R1BQ1RK1 w kq - 0 11", 0, -1, {0}},
	// {"Middlegame 32", "r1b2rk1/pp1n1pbp/1qp2np1/2p1p3/3PP3/2N2NPP/PP3PB1/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 33", "r2q1rk1/1b1nppbp/2pp1np1/pp6/4PP2/2NP2PP/PPP1N1BK/R1BQ1R2 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 34", "r1bq1rk1/p4pbp/2p1pnp1/3p4/3P4/2N3P1/PP2PPBP/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 35", "r2qkbnr/pp1n1pp1/2p1p2p/7P/3P4/3Q1NN1/PPP2PP1/R1B1K2R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 36", "r1bq1rk1/pp1nppbp/3p2p1/8/2P5/2NQ2P1/PP2PPBP/R1B2RK1 w - - 2 11", 0, -1, {0}},
	// {"Middlegame 37", "r1bq1rk1/pppnn1bp/3p2p1/3Ppp2/2P1P3/2NN4/PP2BPPP/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 38", "r2q1rk1/ppp1bppp/2n2n2/6B1/2NPp3/2N5/PPPQbPPP/2KR3R w - - 0 11", 0, -1, {0}},
	// {"Middlegame 39", "r2qk2r/1pp1bpp1/p1n1bn1p/4p3/P7/3P1NP1/1PQ1PPBP/RNB2RK1 w kq - 1 11", 0, -1, {0}},
	// {"Middlegame 40", "1r1q1rk1/1p1bppbp/p1np1np1/2p5/2PP4/1PN1P1PP/P3NPB1/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 41", "r1bqk2r/5pbp/p1np1p2/1p1Np3/4P3/N7/PPP2PPP/R2QKB1R w KQkq - 2 11", 0, -1, {0}},
	// {"Middlegame 42", "r1b1kb1r/2qn1ppp/p1p1p3/3pP3/5P2/2NB4/PPP3PP/R1BQ1RK1 w kq - 1 11", 0, -1, {0}},
	// {"Middlegame 43", "rnbq1rk1/pp3ppp/4p3/8/3PP3/5N2/P2Q1PPP/R3KB1R w KQ - 1 11", 0, -1, {0}},
	// {"Middlegame 44", "r1b2rk1/pp2bppp/2n1pn2/q2p4/2P2B2/P1N1PN2/1PQ2PPP/3RKB1R w K - 5 11", 0, -1, {0}},
	// {"Middlegame 45", "r3kb1r/1bqn1ppp/p2ppn2/1p6/3NPP2/P1N1B3/1PP1B1PP/R2Q1RK1 w kq - 1 11", 0, -1, {0}},
	// {"Middlegame 46", "r1b1k2r/2q1bppp/p1nppn2/1p4B1/4PP2/1NN2Q2/PPP3PP/2KR1B1R w kq - 0 11", 0, -1, {0}},
	// {"Middlegame 47", "r1bq1rk1/4bppp/p2p1n2/npp1p3/4P3/2P2N1P/PPBP1PP1/RNBQR1K1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 48", "r1b1kb1r/1p3ppp/p2qpn2/8/2BN4/8/PPP2PPP/R1BQ1RK1 w kq - 0 11", 0, -1, {0}},
	// {"Middlegame 49", "r1bq1rk1/ppp2ppp/2n5/2b1P3/8/2pP1N2/P1P1B1PP/R1BQ1R1K w - - 0 11", 0, -1, {0}},
	// {"Middlegame 50", "r1br1nk1/ppq1bppp/2pp1n2/4p3/2PPP3/2N1BNPP/PP3PB1/R2Q1RK1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 51", "r2q1rk1/pp1bppbp/3p1np1/8/2PnP3/2N1BP2/PP2B1PP/R2Q1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 52", "rn1qr1k1/pp3ppp/2pb1n2/3p1b2/3P4/P1N1P1P1/1P2NPBP/R1BQ1RK1 w - - 2 11", 0, -1, {0}},
	// {"Middlegame 53", "r1bq1rk1/1p1nbppp/1npp4/p3p3/P2PP3/2N2N1P/BPP2PP1/R1BQR1K1 w - - 4 11", 0, -1, {0}},
	// {"Middlegame 54", "r1bq1rk1/pp2ppbp/2n3p1/2p1n3/2P5/2NN2P1/PP2PPBP/R1BQ1RK1 w - - 6 11", 0, -1, {0}},
	// {"Middlegame 55", "r1b2rk1/1pqnbppp/p2ppn2/8/P2NP3/2N1B1P1/1PP2PBP/R2Q1RK1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 56", "2rq1rk1/pb1nbppp/1p2pn2/2pp4/2PP4/1P2PNP1/PB1N1PBP/R2Q1RK1 w - - 2 11", 0, -1, {0}},
	// {"Middlegame 57", "r2qk1nr/1p3ppp/p1npb3/3Np3/2P1P3/3B4/PP3PPP/RNbQ1RK1 w kq - 0 11", 0, -1, {0}},
	// {"Middlegame 58", "r1bq1rk1/p3ppbp/2p3p1/3n4/8/2N1BP2/PPPQ2PP/R3KB1R w KQ - 0 11", 0, -1, {0}},
	// {"Middlegame 59", "r2qkb1r/pp1b1p2/3p1p1p/1Bp5/4P3/5N2/PPP2PPP/R2QK2R w KQkq - 0 11", 0, -1, {0}},
	// {"Middlegame 60", "r2q1rk1/1p1nbppp/p2pbn2/4p3/4P3/1NN1BP2/PPPQ2PP/2KR1B1R w - - 5 11", 0, -1, {0}},
	// {"Middlegame 61", "r1bqr1k1/ppp2pbp/2np2p1/7n/2PNP3/2N1BP2/PP2B1PP/R2Q1RK1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 62", "r1bq1rk1/2pnbppp/p2p1n2/1p2p3/3PP3/1BP2N1P/PP3PP1/RNBQR1K1 w - - 1 11", 0, -1, {0}},
	// {"Middlegame 63", "r1b1k2r/2qnbppp/p2ppn2/1p4B1/3NPPP1/2N2Q2/PPP4P/2KR1B1R w kq - 0 11", 0, -1, {0}},
	// {"Middlegame 64", "r1bqr1k1/pp1n1pbp/2pp1np1/8/2PNP3/2N3PP/PP3PB1/R1BQ1RK1 w - - 0 11", 0, -1, {0}},
	// {"Middlegame 65", "r1b1qrk1/ppp2pbn/n2p2p1/3Pp1Bp/2P1P2P/2N3N1/PP2BPP1/R2QK2R w KQ - 2 11", 0, -1, {0}},
	// {"Middlegame 66", "2kr1b1r/ppp2ppp/2n1b3/q5N1/3p4/5N2/PPP2PPP/R1BQR1K1 w - - 3 11", 0, -1, {0}},
	// {"Middlegame 67", "r1bqk2r/ppppnppp/5b2/3P4/2B1n3/5N2/PP3PPP/R1BQR1K1 w kq - 3 11", 0, -1, {0}},
	// {"Middlegame 68", "r1bq1rk1/pp2ppbp/2p2np1/8/3PB3/5N1P/PPP2PP1/R1BQR1K1 w - - 5 11", 0, -1, {0}},
	// {"Middlegame 69", "r4rk1/pp1qppbp/2np1np1/8/2PNP3/2N1BP2/PP4PP/R2QK2R w KQ - 1 11", 0, -1, {0}},
	// {"Middlegame 70", "2r2rk1/ppp1ppbp/n3b1p1/3q4/3P1B2/1P2PN2/P4PPP/2RQKB1R w K - 3 11", 0, -1, {0}},
	// {"Middlegame 71", "r1bq1rk1/pp3ppp/1nnbp3/8/3P4/1B3N2/PP3PPP/RNBQ1RK1 w - - 1 11", 0, -1, {0}}
};

#define PERFT_CASE_COUNT (sizeof(PERFT_CASES) / sizeof(PERFT_CASES[0]))

static void loadPerftCase(const perftCase* c, bitboard* board, bool* tomove){
	int temp;
	setboardFEN((char*)c->fen, board, tomove, &temp, &temp);
}

/*
 * ASSERTION TEST
 * */
bool makePerfTestsAssert(){
	initializeAll();

	bitboard bboard;
	bool tomove;
	for (size_t c = 0; c < PERFT_CASE_COUNT; c++){
		const perftCase* pc = &PERFT_CASES[c];
		printf("\n%s\n", pc->name);
		loadPerftCase(pc, &bboard, &tomove);
		printBitBoard2d(stdout, bboard);

		for (int d = pc->minDepth; d <= pc->maxDepth; d++){
			int allmovecount = perfTest(&bboard, tomove, d);
			printf("%d ply:\nLeaves:\t%d\nExp:\t%d\n\n", d, allmovecount, pc->leaves[d]);
			if (allmovecount != pc->leaves[d]) {
				printf("Perft tests failed.\n");
				return false;
			}
		}
	}

	printf("All perft tests passed.\n");
	return true;
}


/*
 * PSEUDO LEGALITY VALIDATOR TEST
 * */
static bool isMoveInList(const movearray* moves, const bitMove* m){
	for (int i = 0; i < moves->size; i++){
		if (sameMove(&moves->array[i], m)) { return true; }
	}
	return false;
}

/* At every node:
 *  - every generated pseudo legal move must pass isPseudoLegal
 *  - the grandparent's moves (same side to move, but a different position)
 *    must pass isPseudoLegal if and only if they were generated here too.
 *    These act as hints from a "wrong" position, like a TT collision.
 * */
static bool pseudoTest(bitboard* board, bool tomove, int depth, const movearray* grandparentMoves, const movearray* parentMoves){
	movearray moves;
	generatePseudoLegalMoves(&moves, board, tomove);

	for (int i = 0; i < moves.size; i++){
		if (!isPseudoLegal(board, tomove, &moves.array[i])){
			printf("Generated move rejected by isPseudoLegal:\n");
			printBitBoard2d(stdout, *board);
			printBitMove(stdout, moves.array[i]);
			printf("\n");
			return false;
		}
	}

	if (grandparentMoves != NULL){
		for (int i = 0; i < grandparentMoves->size; i++){
			const bitMove* m = &grandparentMoves->array[i];
			bool expected = isMoveInList(&moves, m);
			if (isPseudoLegal(board, tomove, m) != expected){
				printf("isPseudoLegal %s a move it should %s:\n", expected ? "rejected" : "accepted", expected ? "accept" : "reject");
				printBitBoard2d(stdout, *board);
				printBitMove(stdout, *m);
				printf("\n");
				return false;
			}
		}
	}

	if (depth == 0){
		return true;
	}

	for (int i = 0; i < moves.size; i++){
		if (!isCastlingLegal(board, tomove, &moves.array[i])){
			continue;
		}
		bitUndo u = makeMove(board, &moves.array[i]);
		if (bitInCheck(board, tomove)){
			undoMove(board, &moves.array[i], &u);
			continue;
		}

		bool ok = pseudoTest(board, !tomove, depth-1, parentMoves, &moves);
		undoMove(board, &moves.array[i], &u);
		if (!ok) { return false; }
	}
	return true;
}

bool testPseudoLegal(){
	initializeAll();

	bitboard board;
	bool tomove;
	for (size_t c = 0; c < PERFT_CASE_COUNT; c++){
		printf("%s: %s\n", PERFT_CASES[c].name, PERFT_CASES[c].fen);
		loadPerftCase(&PERFT_CASES[c], &board, &tomove);
		if (!pseudoTest(&board, tomove, 3, NULL, NULL)){
			printf("isPseudoLegal test failed\n");
			return false;
		}
	}
	printf("isPseudoLegal tests passed\n");
	return true;
}

/*
 * MOVE PICKER TEST
 * */
static const int s_testHistory[12][64] = {{0}};

/* Perft, but the moves come from the move picker. Beside the leaf count, at
 * every node it checks, that the picker:
 *  - returns only pseudo legal moves (an invalid hint must never come out)
 *  - never returns the same move twice (the hint must not be repeated)
 *  - returns exactly as many moves as the plain generator (evasions in check)
 *
 * hint: the move the grandparent played before the current line. It is the
 * same side to move, but a different position, so it is valid only sometimes.
 * */
static long pickerPerft(bitboard* board, bool tomove, int depth, const bitMove* hint, const bitMove* childHint, bool* ok){
	if (depth == 0){
		return 1;
	}

	MovePicker picker;
	initMovePicker(&picker, hint, hint, &NULLBITMOVE, s_testHistory, bitInCheck(board, tomove), false);

	movearray returned = { .size = 0 };
	long all = 0;
	bitMove prevMove = NULLBITMOVE;
	bitMove m;
	while (nextMove(&picker, board, tomove, &m)){
		if (!isPseudoLegal(board, tomove, &m)){
			printf("Move picker returned an invalid move:\n");
			printBitBoard2d(stdout, *board);
			printBitMove(stdout, m);
			printf("\n");
			*ok = false;
			return 0;
		}
		if (isMoveInList(&returned, &m)){
			printf("Move picker returned a move twice:\n");
			printBitBoard2d(stdout, *board);
			printBitMove(stdout, m);
			printf("\nhint: ");
			printBitMove(stdout, *hint);
			printf("\n");
			*ok = false;
			return 0;
		}
		returned.array[returned.size++] = m;

		if (!isCastlingLegal(board, tomove, &m)){
			continue;
		}
		bitUndo u = makeMove(board, &m);
		if (bitInCheck(board, tomove)){
			undoMove(board, &m, &u);
			continue;
		}

		all += pickerPerft(board, !tomove, depth-1, childHint, &prevMove, ok);
		undoMove(board, &m, &u);
		if (!*ok) { return 0; }

		prevMove = m;
	}

	/* In check the picker returns the evasions, and the hint if it is valid.
	 * The hint is not necessarily an evasion, then it comes on top of them. */
	movearray generated;
	generateMovesWithModes(&generated, board, tomove, picker.inCheck ? GEN_EVASIONS : GEN_ALL);
	int expected = generated.size;
	if (isPseudoLegal(board, tomove, hint) && !isMoveInList(&generated, hint)){
		expected++;
	}
	if (expected != returned.size){
		printf("Move picker returned %d moves, expected %d\n", returned.size, expected);

		#ifdef DEBUG
		printf("generated:\n");
		printLegalmoves(generated, *board);
		printf("Move picker:\n");
		printLegalmoves(returned, *board);
		#endif

		printBitBoard2d(stdout, *board);
		*ok = false;
		return 0;
	}

	return all;
}

bool testMovePicker(){
	initializeAll();

	//kiwipete and position 4: castling, en passant, promotions and checks
	const perftCase* cases[] = { &PERFT_CASES[1], &PERFT_CASES[3] };

	bitboard board;
	bool tomove;
	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++){
		const perftCase* pc = cases[i];
		printf("%s: %s\n", pc->name, pc->fen);
		loadPerftCase(pc, &board, &tomove);

		bool ok = true;
		long leaves = pickerPerft(&board, tomove, pc->maxDepth, &NULLBITMOVE, &NULLBITMOVE, &ok);
		printf("%d ply:\nLeaves:\t%ld\nExp:\t%d\n\n", pc->maxDepth, leaves, pc->leaves[pc->maxDepth]);
		if (!ok || leaves != pc->leaves[pc->maxDepth]){
			printf("Move picker test failed\n");
			return false;
		}
	}
	printf("Move picker tests passed\n");
	return true;
}

/*
 * SEARCH SPEED TEST
 * Simulates the GUI sending, for every test position:
 *   ucinewgame
 *   position fen <fen>
 *   go depth <depth>
 * and measures the time and the searched nodes.
 * The history table is not cleared by ucinewgame, so it carries over between
 * the positions (aged), just like in a real game. The order of the positions
 * is fixed, so the runs are still comparable with each other.
 * */
void testSearchSpeed(int depth){
	initializeAll();
	setbuf(stdout, NULL); //same as in the UCI loop, so the info lines show up live

	bitboard board;
	bool tomove;
	int fmv, movenum;
	char positionCmd[256];
	char goCmd[32];
	snprintf(goCmd, sizeof(goCmd), "go depth %d", depth);

	long times[PERFT_CASE_COUNT];
	u64 nodes[PERFT_CASE_COUNT];
	long totalTime = 0;
	u64 totalNodes = 0;

	for (size_t c = 0; c < PERFT_CASE_COUNT; c++){
		const perftCase* pc = &PERFT_CASES[c];
		printf("\n%s\n", pc->name);

		//ucinewgame
		tomove = white;
		clearTransTable();

		snprintf(positionCmd, sizeof(positionCmd), "position fen %s", pc->fen);
		parsePosition(positionCmd, &board, &tomove, &fmv, &movenum);

		long start = getTime_ms();
		parseGo(goCmd, &board, &tomove);
		times[c] = getTime_ms() - start;
		nodes[c] = getSearchedNodes();

		totalTime += times[c];
		totalNodes += nodes[c];
	}

	printf("\nSearch speed, depth %d\n", depth);
	printf("%-22s %10s %12s %10s\n", "position", "time (ms)", "nodes", "knps");
	for (size_t c = 0; c < PERFT_CASE_COUNT; c++){
		printf("%-22s %10ld %12llu %10llu\n", PERFT_CASES[c].name, times[c], nodes[c],
			times[c] > 0 ? nodes[c] / (u64)times[c] : 0);
	}
	printf("%-22s %10ld %12llu %10llu\n", "total", totalTime, totalNodes,
		totalTime > 0 ? totalNodes / (u64)totalTime : 0);
}

#define benchdepth 4

/*
 * BENCHMARK
 * */
void makePerfTest(bitboard* board, bool tomove){
	printLogo();
	for (int i = 0; i < benchdepth; i++){
		int allmovecount = perfTest(board, tomove, i);
		printf("%d ply:\nLeaves: %d\n\n", i+1, allmovecount);
	}
}


bool assertEval(){
	// 

	initializeAll();

	printf("Asert evaluation...\n");

	bitboard bboard;
	bool tomove;

	loadPerftCase(&PERFT_CASES[0], &bboard, &tomove);

	movearray moves;
	moves.size = 1;
	for (int i = 0; i < 80 && moves.size; i++){
		generateLegalMoves(&moves, &bboard, tomove);
		if (moves.size){
			//make random legal move
			makeMove(&bboard, &moves.array[(i * 179) % moves.size]);
			tomove = !tomove;
			
			if (staticEval(&bboard, white) != -staticEval(&bboard, black)){
				printf("Evaluations differ.\n");
				printBitBoard2d(stdout, bboard);
				return false;	
			}
		}
	}

	// printBitBoard2d(stdout, bboard);
	printf("Evaluations were symmetric\n");

	return true;
}
