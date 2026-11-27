#include "headers/main.h"

FILE* g_debugOutput;

//test run:  ./hedgine test

#ifdef DEBUG
long int runStartTime;
#endif

int main(int argc, char* argv[]){
	initmagicmoves();

	#ifdef DEBUG 
	runStartTime = getTime_ms();
	g_debugOutput = fopen("debuglog.txt", "w");
	if (g_debugOutput == NULL){
		return 0;
	}

	for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
	#endif

	/**
	 * ./hedgine <test> [arguments]
	 * Every test exits when it's done, passed or not. Any other first argument
	 * (or none) starts the engine as usual.
	 */
	if (argc >= 2){
		bool isTest = true;

		if (strncmp(argv[1], "test", 5) == 0){
			makePerfTestsAssert();
		}
		else if (strncmp(argv[1], "testLasker", 11) == 0){
			testLasker();
		}
		else if (strncmp(argv[1], "testEnglish", 12) == 0){
			testEnglish();
		}
		else if (strncmp(argv[1], "testPseudo", 11) == 0){
			testPseudoLegal();
		}
		else if (strncmp(argv[1], "testMovePicker", 15) == 0){
			testMovePicker();
		}
		else if (strncmp(argv[1], "testSearch", 11) == 0){
			//./hedgine testSearch [depth]
			int depth = (argc >= 3) ? atoi(argv[2]) : 6;
			if (depth < 1) depth = 4;
			testSearchSpeed(depth);
		}
		else if (strncmp(argv[1], "testMakeMove", 13) == 0){
			makeMoveTest();
		}
		else if (strncmp(argv[1], "testNNUE", 9) == 0){
			#ifdef USE_NNUE
			//./hedgine testNNUE net128.bin net128.golden.txt
			if (argc < 4) {
				printf("usage: hedgine testNNUE <net.bin> <net.golden.txt>\n");
			}
			else if (NNUEload(argv[2])){
				NNUEgoldenTest(argv[3]);
			}
			#else
			printf("NNUE not available.\n");
			#endif
		}
		else if (strncmp(argv[1], "testNNUEInc", 12) == 0){
			#ifdef NNUE_INCREMENTAL
			//./hedgine testNNUEInc [net.bin]   (the embedded net if no file is given)
			if (argc >= 3 ? NNUEload(argv[2]) : NNUEloadEmbedded()) {
				NNUEincrementalTest();
			}
			#else
			printf("Incremental NNUE not compiled in (make NNUE=1).\n");
			#endif
		}
		else if (strncmp(argv[1], "testFifty", 10) == 0){
			testFiftyMoveCounter();
		}
		else if (strncmp(argv[1], "testAll", 8) == 0){
			if (!(makeMoveTest() && 
				testMovePicker() && 
				testPseudoLegal() && 
				makePerfTestsAssert() && 
				#ifndef NNUE_INCREMENTAL
				assertEval() && 
				#endif
				testFiftyMoveCounter()
				#ifdef NNUE_INCREMENTAL
				&& NNUEloadEmbedded()
				&& NNUEincrementalTest()
				#endif
			)) {
				printf("fail, quitting\n");
			}
		}
		else {
			isTest = false;
		}

		if (isTest){
			goto endEverything;
		}
	}
	
	initializeAll();
	
	sidetomove tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	
	bitboard board;

	// #ifdef DEBUG	
	// setboardFEN("4k3/8/8/3p2pP/8/2N4p/2q4P/4K2R w K g6 0 1", &board, &tomove, &fmv, &movenum);
	// printBitBoard2d(stdout, board);
	// movearray moves;
	// generateLegalMoves(&moves, &board, tomove);
	// for (int i = 0; i < moves.size; i++){
	// 	printBitMove(stdout, moves.array[i]);
	// 	printf("\n");
	// }
	// #endif
	
	//main uci loop
	UCIloop(&board, &tomove, &fmv, &movenum);
	
	endEverything:
	
	#ifdef DEBUG
	printf("runtime: %ld\n", getTime_ms() - runStartTime);
	printCollisionStats();	
	fclose(g_debugOutput);
	#endif
	
	freeTransTable();
	
	return 0;
}
