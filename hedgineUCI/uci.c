#include "uci.h"

gameInfo info;

/* 
 * UCI
 * forked from BBC
 * by Maksim Korzh
 * which is
 * forked from VICE
 * by Richard Allbert
 */
 
// parse UCI "position" command
void parsePosition(char *command, bitboard* board, bool *tomove, int* fmv, int* movenum){
	// init pointer to the current character in the command string
	char *current_char = command+9;//shifted 9 from the position token 
	
	
	// parse UCI "startpos" command
	if (strncmp(command+9, "startpos", 8) == 0){
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, tomove, fmv, movenum);
	}
	else { // parse UCI "fen" command 
		// make sure "fen" command is available within command string
		current_char = strstr(command, "fen");
		
		// if no "fen" command is available within command string
		if (current_char == NULL){
			// init chess board with start position, nothing was specified
			setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, tomove, fmv, movenum);
		}
		else{
			// shift pointer to the right where next token begins
			current_char += 4;
			
			// init chess board with position from FEN string
			readFEN(current_char, board, tomove, fmv, movenum);
			//~ #ifdef DEBUG
			//~ printBitBoard2d(*board);
			//~ #endif
		}
	}
	
	current_char = strstr(command, "moves");
	
	// moves available
	if (current_char != NULL){
		// shift pointer to the right where next token begins
		current_char += 6;
		
		// loop over moves within a move string
		while(*current_char != 0){
			// parse next move
			move m = parseLongAlgebraicNotation(current_char);
			
			// if no more moves
			if (m.from.file == -1) break;
			
			//~ repetition_index++;
			//~ repetition_table[repetition_index] = hash_key;
			
			// make move on the chess board
			int x = isMoveLegal(board, *tomove, m);
			if (x == 1){
				//the move was illegal
				info.quit = true;
				fprintf(stderr, TXT_RED "Ilegal move found!\n" DEFAULT);
				break;
			}
			else if (x == 2){
				//no legal moves left
				info.quit = true;
				fprintf(stderr, TXT_RED "No legal moves in position!\n" DEFAULT);
				break;
			}
			
			if (*tomove == black) (*movenum)++;
			*tomove = !(*tomove);
			
			// move current character mointer to the end of current move
			while (*current_char && *current_char != ' ') current_char++;
			
			// go to the next move
			current_char++;
		}		
	}
	
	#ifdef DEBUG
	printf("debug\ttomove: %d\tfifty move count: %d\tmove num: %d\n", *tomove, *fmv, *movenum);
	//~ printBitBoard2d(*board);
	#endif
}

// reset time control variables
void resetTimeControl(){
	info.timeControl = true;
	info.moveTime = 1234;
}

// parse UCI command "go"
void parseGo(char *command, bitboard* board, bool *tomove){
	// reset time control
	resetTimeControl();

	// init argument
	char *argument = NULL;

	// infinite search
	if ((argument = strstr(command,"infinite"))) {
		info.timeControl = false;
	}

	// match UCI increments: doesn't change program behacvior at the moment
	if ((argument = strstr(command,"binc"))) {
		info.timeControl = true;
	}
	if ((argument = strstr(command,"winc"))) {
		info.timeControl = true;
	}
	
	
	if ((argument = strstr(command,"wtime")) && *tomove == white) {
		info.timeRemaining = atoi(argument + 6);
		setMoveTime();
		info.timeControl = true;
	}

	if ((argument = strstr(command,"btime")) && *tomove == black) {
		info.timeRemaining = atoi(argument + 6);
		setMoveTime();
		info.timeControl = true;
	}


	if ((argument = strstr(command,"movestogo"))){
		info.timeControl = true;
	}

	if ((argument = strstr(command,"movetime"))) {
		info.timeControl = true;
		info.moveTime = atoi(argument + 9);
	}

	int cpulvl = 40;
	if ((argument = strstr(command,"depth")))
		// parse search depth
		cpulvl = atoi(argument + 6);

	// init start time
	info.startTime = getTime_ms();

	#ifdef DEBUG
	// print debug info
	printf("debug\ttime control %d\tstart time: %ld\tmoveTime: %d \tdepth: %d\n", info.timeControl, info.startTime, info.moveTime, cpulvl);
	printBitBoard2d(*board);
	#endif
	
	move m = CPU(cpulvl, *board, *tomove);
	printf("bestmove ");
	printmove(m);
	printf("\n");
	fflush(stdout);
}



// main UCI loop
void UCIloop(bitboard* board, bool *tomove, int* fmv, int* movenum) {
	// reset STDIN & STDOUT buffers
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	
	// define user / GUI input buffer
	char* input = NULL;
	
	// print engine info
	//~ printf("id name Hedgine\n");
	//~ printf("id author B.M.\n");
	//~ printf("option name Hash type spin default %d min %d max %d\n", TT_DEF_SIZE_MB, TT_MIN_SIZE_MB, TT_MAX_SIZE_MB);
	//~ printf("uciok\n");
	
	// main loop
	while (!info.quit) {		
		// make sure output reaches the GUI
		fflush(stdout);
		
		if (info.newgame){
			parsePosition("position startpos", board, tomove, fmv, movenum);
			clearTransTable();
			info.newgame = false;
		}
		
		int temp = getLineDynamic(&input, 1000);
		if (temp == 0){
			if (input != NULL){
				free(input);
			}
			input = NULL;
			continue;
		}
		else if (strncmp(input, "isready", 7) == 0){
			printf("readyok\n");
		}
		else if (strncmp(input, "position", 8) == 0) {
			parsePosition(input, board, tomove, fmv, movenum);
		
			//~ clearTransTable();
			//~ printBitBoard2d(*board);
		}
		else if (strncmp(input, "ucinewgame", 10) == 0) {
			parsePosition("position startpos", board, tomove, fmv, movenum);
			
			clearTransTable();
			#ifdef DEBUG
			printBitBoard2d(*board);
			#endif
		}
		else if (strncmp(input, "go", 2) == 0){
			parseGo(input, board, tomove);
		}
		else if (strncmp(input, "quit", 4) == 0){
			info.quit = true;
		}
		else if (strncmp(input, "uci", 3) == 0)	{
			// print engine info
			printf("id name Hedgine\n");
			printf("id author B.M.\n");
			printf("option name Hash type spin default %d min %d max %d\n", TT_DEF_SIZE_MB, TT_MIN_SIZE_MB, TT_MAX_SIZE_MB);
			printf("uciok\n");
		}
		else if (!strncmp(input, "setoption name Hash value ", 26)) {
			int mb;
			sscanf(input,"%*s %*s %*s %*s %d", &mb);
			
			if(mb < TT_MIN_SIZE_MB) mb = TT_MIN_SIZE_MB;
			if(mb > TT_MAX_SIZE_MB) mb = TT_MAX_SIZE_MB;
			
			freeTransTable();
			if (allocTransTable( mb ) == NULL){
				exit(1);
			}
			
			printf("info set hash table size to %dMB\n", mb);
		}
		
		if (input != NULL){
			free(input);
		}
		input = NULL;
	}
}

// init all variables
void initializeAll(){
	// init random keys for hashing purposes
	setHashKey();
	
	//set game info
	info.quit = false;
	info.moveTime = -1;
	info.timeControl = false;
	info.newgame = false;

	
	// init hash table with default size
	if (allocTransTable( TT_DEF_SIZE_MB ) == NULL){
		exit(1);
	}
}

// get time in milliseconds
long int getTime_ms(){
	#ifdef WIN64
		return GetTickCount();
	#else
		struct timeval time_value;
		gettimeofday(&time_value, NULL);
		return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
	#endif
}

/*

  Function to "listen" to GUI's input during search.
  It's waiting for the user input from STDIN.
  OS dependent.
 
*/
  
int inputWaiting(){
	#ifndef WIN32
		fd_set readfds;
		struct timeval tv;
		FD_ZERO (&readfds);
		FD_SET (fileno(stdin), &readfds);
		tv.tv_sec=0; tv.tv_usec=0;
		select(16, &readfds, 0, 0, &tv);

		return (FD_ISSET(fileno(stdin), &readfds));
	#else
		static int init = 0, pipe;
		static HANDLE inh;
		DWORD dw;

		if (!init){
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);
			if (!pipe)
			{
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}
		
		if (pipe){
		   if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
		   return dw;
		}
		
		else{
		   GetNumberOfConsoleInputEvents(inh, &dw);
		   return dw <= 1 ? 0 : dw;
		}

	#endif
}

// read GUI/user input
void readInput() {
	// GUI/user input
	char* input = NULL;  
	size_t bytesRead;

	// "listen" to STDIN
	if (inputWaiting()) {		
		// Tell engine to stop calculating
		stopSearch = true;

		bytesRead = getLineDynamic(&input, 100);  
		
		// If input is available
		if (bytesRead > 0) {
			if (strncmp(input, "quit", 4) == 0) {
				stopSearch = true;
				info.quit = true;
			}
			else if (strncmp(input, "stop", 4) == 0) {
				stopSearch = true;
				//~ info.quit = true;
			}
			else if (strncmp(input, "ucinewgame", 10) == 0) {
				stopSearch = true;
				info.newgame = true;
				//~ info.quit = true;
			}
		}
	}
	if (input != NULL) free(input);
}
	
