#include "headers/main.h"

FILE* g_debugOutput;

//test run:  ./hedgine test quit

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

	if (argc >= 2){
		if (strncmp(argv[1], "test", 5) == 0){
			if (!makePerfTestsAssert()){
				goto endEverything;
			}
		}
		else if (strncmp(argv[1], "testLasker", 10) == 0){
			testLasker();
		}
		else if (strncmp(argv[1], "testEnglish", 12) == 0){
			testEnglish();
		}
		else if (strncmp(argv[1], "testPseudo", 11) == 0){
			if (!testPseudoLegal()) { goto endEverything; }
		}
		else if (strncmp(argv[1], "testMovePicker", 15) == 0){
			if (!testMovePicker()) { goto endEverything; }
		}
		else if (strncmp(argv[1], "testSearch", 11) == 0){
			//./hedgine testSearch [depth]
			int depth = (argc >= 3) ? atoi(argv[2]) : 6;
			if (depth < 1) depth = 4;
			testSearchSpeed(depth);
			goto endEverything;
		}
		else if (strncmp(argv[1], "testMakeMove", 15) == 0){
			if (!makeMoveTest()) { goto endEverything; }
		}
		else if (strncmp(argv[1], "testAll", 8) == 0){
			if (!(makeMoveTest() && testMovePicker() && testPseudoLegal() && makePerfTestsAssert())) { 
				printf("fail, quitting\n");
				goto endEverything; 
			}
		}
	}
	if (argc >= 3 && strncmp(argv[2], "quit", 4) == 0){
		goto endEverything;
		return 0;
	}
	
	initializeAll();
	
	bool tomove = white;
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
