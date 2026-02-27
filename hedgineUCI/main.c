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
		if (strncmp(argv[1], "testLasker", 10) == 0){
			testLasker();
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

	//main uci loop
	UCIloop(&board, &tomove, &fmv, &movenum);
	
	endEverything:
	
	#ifdef DEBUG
	fprintf(g_debugOutput, "runtime: %ld\n", getTime_ms() - runStartTime);
	printCollisionStats();	
	fclose(g_debugOutput);
	#endif
	
	freeTransTable();
	
	return 0;
}
