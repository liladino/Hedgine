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
			if (allmovecount != pc->leaves[d]) return false;
		}
	}

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
