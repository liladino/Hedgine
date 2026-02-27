#include "headers/uci.h"

gameInfo info;

static char *skip_ws(char *str) {
	while (*str && isspace((unsigned char)*str)) str++;
	return *str ? str : NULL;
}

static char *skip_token(char *str) {
	while (*str && !isspace((unsigned char)*str)) str++;
	return str;
}

/* Returns start of next token after the current one (or NULL). */
static char* jumpToNextToken(char *str) {
	if (!str) return NULL;
	str = skip_token(str);
	return skip_ws(str);
}

// compare, if first token matches the second AND doesn't extend frther
static inline bool compareToken(const char* str1, const char* str2, size_t n){
	if (!str1) return false;
	return (strncmp(str1, str2, n) == 0 && (str1[n] == '\0' || isspace((unsigned char)str1[n])));
}

static char* findToken(char* str, const char* token) {
	if (!str || !token) return NULL;
	size_t n = strlen(token);

	for (str = skip_ws(str); str; str = jumpToNextToken(str)) {
		if (compareToken(str, token, n)) {
			return str;
		}

	}
	return NULL;
}

/* 
 * UCI communication
 * forked from BBC
 * by Maksim Korzh
 */
 
// parse UCI "position" command
//e.g position startpos moves e2e4 e7e5
//	position fen ... moves ...
void parsePosition(char* command, bitboard* board, bool* tomove, int* fmv, int* movenum){
	char* position = jumpToNextToken(command); // after "position "
	if (!position) return; 
	
	if (strncmp(position, "startpos", 8) == 0){
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, tomove, fmv, movenum);
	}
	else { 
		position = findToken(command, "fen"); 
		//~ position = strstr(command, "fen");
		
		// if no "fen" command is available within command string
		if (position == NULL){
			// init chess board with start position, nothing was specified
			setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, tomove, fmv, movenum);
		}
		else{
			// shift to next token
			position = jumpToNextToken(position);
			
			// init chess board with position from FEN string
			readFEN(position, board, tomove, fmv, movenum);
		}
	}
	//store the start position in the repetition table too
	storeRepetiton(board->hashValue);
	
	char* currentmove = findToken(command, "moves"); //~ strstr(command, "moves");
	if (currentmove == NULL){
		goto print;
	}	
	currentmove = jumpToNextToken(currentmove); //skip the "moves" token
	
	while(currentmove){
		// parse next move
		move m = parseLongAlgebraicNotation(currentmove);
		
		//~ printBitBoard2d(stdout, *board);
		//~ printmove(stdout, m);
		
		// if no more moves
		if (m.from.file == -1) break;
		
		bitboard last = *board; 
		int x = isMoveLegal(board, *tomove, m);
		if (x == 1){
			//the move was illegal
			info.quit = true;
			fprintf(stderr, "Ilegal move found!\n");
			#ifdef DEBUG
			fprintf(g_debugOutput, "Ilegal move found!\n");
			#endif
			break;
		}
		else if (x == 2){
			//no legal moves left
			info.quit = true;
			fprintf(stderr, "No legal moves in position!\n" );
			#ifdef DEBUG
			fprintf(g_debugOutput, "No legal moves in position!\n");
			#endif
			break;
		}
		
		/* store in the repetition table 
		 * 
		 * if the move was a capture, castling, a pawn move, or the en 
		 * passant target square changed(/got deleted), the position can't
		 * be repeated anymore => the write index can be 0 again */
		if (
			last.piece[wpawn] != board->piece[wpawn] || 
			last.piece[bpawn] != board->piece[bpawn] || 
			last.castlerights != board->castlerights || 
			last.enpassanttarget != board->enpassanttarget || 
			lastMoveWasCapture(&last, m, tomove)
				){
			g_RTwriteIndex = 0;
		}
		storeRepetiton(board->hashValue);
		
		if (*tomove == black) (*movenum)++;
		*tomove = !(*tomove);
		
		currentmove = jumpToNextToken(currentmove);
	}
	
print:
	#ifdef DEBUG
	fprintf(g_debugOutput, "tomove: %d\tfifty move count: %d\tmove num: %d\n", *tomove, *fmv, *movenum);
	printBitBoard2d(g_debugOutput, *board);
	#endif
	return; //to surpress warning when not in debug 
}

// reset time control variables
void resetTimeControl(){
	info.timeControl = true;
	info.moveTime = 1234;
}

// parse UCI command "go"
void parseGo(char *command, bitboard* board, bool *tomove) {
	resetTimeControl();

	bool infinite = false;
	int wtime = -1, btime = -1;
	int winc  = 0,  binc  = 0;
	int movetime = -1;
	int depth = 40;

	// Start at first token after "go"
	char *arg = jumpToNextToken(command);
	for (; arg; arg = jumpToNextToken(arg)) {
		char* v = NULL;
		if (compareToken(arg, "infinite", 8)) {
			infinite = true;
		}
		else if (compareToken(arg, "depth", 5)) {
			v = jumpToNextToken(arg);
			if (v) depth = atoi(v);
			if (depth < 1 || depth > 40) depth = 2;
		}
		else if (compareToken(arg, "movetime", 8)) {
			v = jumpToNextToken(arg);
			if (v) movetime = atoi(v);
		}
		else if (compareToken(arg, "wtime", 5)) {
			v = jumpToNextToken(arg);
			if (v) wtime = atoi(v);
		}
		else if (compareToken(arg, "btime", 5)){
			v = jumpToNextToken(arg);
			if (v) btime = atoi(v);
		}
		else if (compareToken(arg, "winc", 4)) {
			v = jumpToNextToken(arg);
			if (v) winc = atoi(v);
		}
		else if (compareToken(arg, "binc", 4)) {
			v = jumpToNextToken(arg);
			if (v) binc = atoi(v);
		}
		
		if (v){
			arg = v;
		}
		// ignore unsupported tokens for now: movestogo, nodes, mate, etc.
	}

	// Apply precedence
	if (infinite) {
		info.timeControl = false;
	} else if (movetime >= 0) {
		info.timeControl = true;
		info.moveTime = movetime;
	} else {
		// time controls
		int inc = (*tomove == white) ? winc : binc;
		int rem = (*tomove == white) ? wtime : btime;
		if (rem >= 0) {
			info.timeControl = true;
			info.timeRemaining = rem;
			setMoveTime(inc);
		}
	}

	// init start time
	info.startTime = getTime_ms();

	#ifdef DEBUG
	fprintf(g_debugOutput, "time control %d\tstart time: %ld\tmoveTime: %d \tdepth: %d\n", info.timeControl, info.startTime, info.moveTime, depth);
	printBitBoard2d(g_debugOutput, *board);
	#endif
	
	move m = CPU(depth, *board, *tomove);
	printf("bestmove ");
	printmove(stdout, m);
	printf("\n");
	fflush(stdout);
	
	#ifdef DEBUG
	fprintf(g_debugOutput, "bestmove: ");
	printmove(g_debugOutput, m);
	fprintf(g_debugOutput, "\n");
	fflush(g_debugOutput);
	#endif
}

// main UCI loop
void UCIloop(bitboard* board, bool *tomove, int* fmv, int* movenum) {
	// reset STDIN & STDOUT buffers
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	
	// input buffer
	char* input = NULL;
	
	// main loop
	while (!info.quit) {		
		// make sure output reaches the GUI
		fflush(stdout);
		
		if (info.newgame){
			//parsePosition("position startpos", board, tomove, fmv, movenum);
			clearTransTable();
			info.newgame = false;
			*tomove = white;
		}
		
		{
			int x = readLineDynamic(&input, 10000);
			if (x == 0){
				continue;
			}
			else if (x == -1) {
				break;
			}
		}
		
		if ((compareToken(input, "isready", 7))) {
			printf("readyok\n");
		}
		else if ((compareToken(input, "position", 8))) {
			parsePosition(input, board, tomove, fmv, movenum);
			//~ clearTransTable();
		}
		else if ((compareToken(input, "ucinewgame", 10))) {
			//parsePosition("position startpos", board, tomove, fmv, movenum);
			*tomove = white;
			clearTransTable();
		}
		else if ((compareToken(input, "go", 2))){
			parseGo(input, board, tomove);
		}
		else if ((compareToken(input, "quit", 4))){
			info.quit = true;
		}
		else if ((compareToken(input, "uci", 3))){
			printf("id name Hedgine\n");
			printf("id author B.M.\n");
			printf("option name Hash type spin default %d min %d max %d\n", TT_DEF_SIZE_MB, TT_MIN_SIZE_MB, TT_MAX_SIZE_MB);
			printf("uciok\n");
		}
		else if (compareToken(input, "setoption", 9)) {
			char* temp = jumpToNextToken(input);
			if (compareToken(temp, "name", 4)){
				if (compareToken(temp = jumpToNextToken(temp), "Hash", 4)){
					if (compareToken(temp = jumpToNextToken(temp), "value", 4)){
						if ((temp = jumpToNextToken(temp))){
							int mb = atoi(temp);
							if(mb < TT_MIN_SIZE_MB) mb = TT_MIN_SIZE_MB;
							if(mb > TT_MAX_SIZE_MB) mb = TT_MAX_SIZE_MB;
							
							freeTransTable();
							if (allocTransTable( mb ) == NULL){
								exit(1);
							}
							
							printf("info set hash table size to %dMB\n", mb);
						}
					}
				}
			}
		}
		
		//~ if (input != NULL){
			//~ free(input);
		//~ }
		//~ input = NULL;
	}
	free(input);
}

// init all variables
void initializeAll(){
	static bool initialized = false;
	if (!initialized){
		initialized = true;
	}
	else{
		return;
	}
	
	// init random keys for hashing purposes
	setHashKey();
	
	//set game info
	info.quit = false;
	info.moveTime = -1;
	info.timeControl = false;
	info.newgame = false;

	
	// init hash table with default size
	if (allocTransTable(TT_DEF_SIZE_MB) == NULL){
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
	int bytesRead;

	// "listen" to STDIN
	if (inputWaiting()) {		
		// Tell engine to stop calculating
		g_stopSearch = true;

		bytesRead = readLineDynamic(&input, 1024);  
		
		// If input is available
		if (bytesRead > 0) {
			if (strncmp(input, "quit", 4) == 0) {
				g_stopSearch = true;
				info.quit = true;
			}
			else if (strncmp(input, "stop", 4) == 0) {
				g_stopSearch = true;
				//~ info.quit = true;
			}
			else if (strncmp(input, "ucinewgame", 10) == 0) {
				g_stopSearch = true;
				info.newgame = true;
				//~ info.quit = true;
			}
		}
	}
	if (input != NULL) free(input);
}
	

void setMoveTime(int increment){
	#define SECOND 1000
	info.moveTime = increment - 100;
	if (info.moveTime < 0) info.moveTime = 0;
	
	if (info.timeRemaining <= 0.5 * SECOND){ //half a sec left
		info.moveTime += 50;
		return;
	}
	if (info.timeRemaining <= SECOND){ //1 sec left
		info.moveTime += 100;
		return;
	}
	if (info.timeRemaining <= 2 * SECOND){ //2 sec left
		info.moveTime += info.timeRemaining / 5; //gets down from .4 sec to .1 sec
		return;
	}
	if (info.timeRemaining <= 10 * SECOND){ //10 sec left
		info.moveTime += 500 + (info.timeRemaining - 2000) / 16; //1 sec and gets down to half sec
		return;
	}
	if (info.timeRemaining <= 30 * SECOND){ //30 sec left
		info.moveTime += 1.5 * SECOND; //1.5 sec
		return;
	}
	if (info.timeRemaining <= 60 * SECOND){ //1 min left
		info.moveTime += 2.1 * SECOND; //2.1 sec
		return;
	}
	if (info.timeRemaining <= 2 * 60 * SECOND){ //2 min left
		info.moveTime += 3.72 * SECOND; //3.7 sec
		return;
	}
	if (info.timeRemaining <= 3 * 60 * SECOND){ //3 min left
		info.moveTime += 5.45 * SECOND; //5.5 sec
		return;
	}
	if (info.timeRemaining <= 5 * 60 * SECOND){ //5 min left
		info.moveTime += 7.91 * SECOND; 
		return;
	}
	if (info.timeRemaining <= 10 * 60 * SECOND){ //10 min left
		info.moveTime += 9.9 * SECOND; 
		return;
	}
	if (info.timeRemaining <= 20 * 60 * SECOND){ //20 min left
		info.moveTime += 15 * SECOND; 
		return;
	}
	if (info.timeRemaining <= 45 * 60 * SECOND){ //45 min left
		info.moveTime += 30 * SECOND; 
		return;
	}
	info.moveTime += 40 * SECOND; 	
}
