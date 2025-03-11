#include "main.h"

FILE* debugOutput;

//test run:  ./hedgine test quit

int main(int argc, char* argv[]){
	initmagicmoves();
	
	#ifdef DEBUG
	long int runStartTime = getTime_ms();
	debugOutput = fopen("debuglog.txt", "w");
	if (debugOutput == NULL){
		return 0;
	}
	#endif
		
	for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
	if (argc >= 2 && strncmp(argv[1], "test", 4) == 0){
		if (makePerfTestsAssert()){
			printf("All tests passed.\n");
		}
		else{
			printf("Tests failed.\n");
			goto endEverything;
			return 0;
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
	
	
	freeTransTable();
	
	endEverything:
	#ifdef DEBUG
	fprintf(debugOutput, "runtime: %ld\n", getTime_ms() - runStartTime);
	fclose(debugOutput);
	#endif
	
	return 0;
}
