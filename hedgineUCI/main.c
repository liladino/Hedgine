#include "headers/main.h"

FILE* g_debugOutput;

//test run:  ./hedgine test quit

int main(int argc, char* argv[]){
	initmagicmoves();
	
	#ifdef DEBUG
	long int runStartTime = getTime_ms();
	g_debugOutput = fopen("debuglog.txt", "w");
	if (g_debugOutput == NULL){
		return 0;
	}
	
	printDebug("qwertyuiop\nasdfghjkl\nzxcvbnm\n%d%c", 100, 'a');
	printDebug("\n");
	#endif
		
	for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
	if (argc >= 2){
		if (strncmp(argv[1], "test", 5) == 0){
			if (makePerfTestsAssert()){
				printf("All tests passed.\n");
			}
			else{
				printf("Tests failed.\n");
				goto endEverything;
				return 0;
			}
		}
		else if (strncmp(argv[1], "testLasker", 10) == 0){
			testLasker();
		}
		else if (strncmp(argv[1], "testEnglish", 12) == 0){
			testEnglish();
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

	#ifdef DEBUG	
	setboardFEN("4k3/8/8/3p2pP/8/2N4p/2q4P/4K2R w K g6 0 1", &board, &tomove, &fmv, &movenum);
	printBitBoard2d(stdout, board);
	movearray moves;
	bitGenerateLegalmoves(&moves, &board, tomove, false);
	for (int i = 0; i < moves.size; i++){
		printBitMove(stdout, moves.array[i]);
		printf("\n");
	}
	#endif
	
	//main uci loop
	UCIloop(&board, &tomove, &fmv, &movenum);
	
	endEverything:
	
	#ifdef DEBUG
	printDebug("runtime: %ld\n", getTime_ms() - runStartTime);
	printCollisionStats();	
	fclose(g_debugOutput);
	#endif
	
	freeTransTable();
	
	return 0;
}
