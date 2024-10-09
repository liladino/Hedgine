#include "main.h"

FILE* debugOutput;

int main(){
	#ifdef DEBUG
	long int runStartTime = getTime_ms();
	debugOutput = fopen("debuglog.txt", "w");
	if (debugOutput == NULL) return 0;
	#endif
	
	initializeAll();
	
	bool tomove = white;
	int movenum = 1, fmv = 0; //fifty-move rule
	
	bitboard board;

	//main uci loop
	UCIloop(&board, &tomove, &fmv, &movenum);
			
	freeTransTable();
	
	#ifdef DEBUG
	fprintf(debugOutput, "runtime: %ld\n", getTime_ms() - runStartTime);
	fclose(debugOutput);
	#endif
	
	return 0;
}
