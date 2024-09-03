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
 
//~ // parse UCI "position" command
//~ void parsePosition(char *command)
//~ {
	//~ // shift pointer to the right where next token begins
	//~ command += 9;
	
	//~ // init pointer to the current character in the command string
	//~ char *current_char = command;
	
	//~ // parse UCI "startpos" command
	//~ if (strncmp(command, "startpos", 8) == 0)
		//~ // init chess board with start position
		//~ parse_fen(start_position);
	
	//~ // parse UCI "fen" command 
	//~ else
	//~ {
		//~ // make sure "fen" command is available within command string
		//~ current_char = strstr(command, "fen");
		
		//~ // if no "fen" command is available within command string
		//~ if (current_char == NULL)
			//~ // init chess board with start position
			//~ parse_fen(start_position);
			
		//~ // found "fen" substring
		//~ else
		//~ {
			//~ // shift pointer to the right where next token begins
			//~ current_char += 4;
			
			//~ // init chess board with position from FEN string
			//~ parse_fen(current_char);
		//~ }
	//~ }
	
	//~ // parse moves after position
	//~ current_char = strstr(command, "moves");
	
	//~ // moves available
	//~ if (current_char != NULL)
	//~ {
		//~ // shift pointer to the right where next token begins
		//~ current_char += 6;
		
		//~ // loop over moves within a move string
		//~ while(*current_char)
		//~ {
			//~ // parse next move
			//~ int move = parse_move(current_char);
			
			//~ // if no more moves
			//~ if (move == 0)
				//~ // break out of the loop
				//~ break;
			
			//~ // increment repetition index
			//~ repetition_index++;
			
			//~ // wtire hash key into a repetition table
			//~ repetition_table[repetition_index] = hash_key;
			
			//~ // make move on the chess board
			//~ make_move(move, all_moves);
			
			//~ // move current character mointer to the end of current move
			//~ while (*current_char && *current_char != ' ') current_char++;
			
			//~ // go to the next move
			//~ current_char++;
		//~ }		
	//~ }
	
	//~ // print board
	//~ print_board();
//~ }

//~ // reset time control variables
//~ void reset_time_control()
//~ {
	//~ // reset timing
	//~ quit = 0;
	//~ movestogo = 30;
	//~ movetime = -1;
	//~ time = -1;
	//~ inc = 0;
	//~ starttime = 0;
	//~ stoptime = 0;
	//~ timeset = 0;
	//~ stopped = 0;
//~ }

//~ // parse UCI command "go"
//~ void parse_go(char *command)
//~ {
	//~ // reset time control
	//~ reset_time_control();
	
	//~ // init parameters
	//~ int depth = -1;

	//~ // init argument
	//~ char *argument = NULL;

	//~ // infinite search
	//~ if ((argument = strstr(command,"infinite"))) {}

	//~ // match UCI "binc" command
	//~ if ((argument = strstr(command,"binc")) && side == black)
		//~ // parse black time increment
		//~ inc = atoi(argument + 5);

	//~ // match UCI "winc" command
	//~ if ((argument = strstr(command,"winc")) && side == white)
		//~ // parse white time increment
		//~ inc = atoi(argument + 5);

	//~ // match UCI "wtime" command
	//~ if ((argument = strstr(command,"wtime")) && side == white)
		//~ // parse white time limit
		//~ time = atoi(argument + 6);

	//~ // match UCI "btime" command
	//~ if ((argument = strstr(command,"btime")) && side == black)
		//~ // parse black time limit
		//~ time = atoi(argument + 6);

	//~ // match UCI "movestogo" command
	//~ if ((argument = strstr(command,"movestogo")))
		//~ // parse number of moves to go
		//~ movestogo = atoi(argument + 10);

	//~ // match UCI "movetime" command
	//~ if ((argument = strstr(command,"movetime")))
		//~ // parse amount of time allowed to spend to make a move
		//~ movetime = atoi(argument + 9);

	//~ // match UCI "depth" command
	//~ if ((argument = strstr(command,"depth")))
		//~ // parse search depth
		//~ depth = atoi(argument + 6);

	//~ // if move time is not available
	//~ if(movetime != -1)
	//~ {
		//~ // set time equal to move time
		//~ time = movetime;

		//~ // set moves to go to 1
		//~ movestogo = 1;
	//~ }

	//~ // init start time
	//~ starttime = get_time_ms();

	//~ // init search depth
	//~ depth = depth;

	//~ // if time control is available
	//~ if(time != -1)
	//~ {
		//~ // flag we're playing with time control
		//~ timeset = 1;

		//~ // set up timing
		//~ time /= movestogo;
		
		//~ // disable time buffer when time is almost up
		//~ if (time > 1500) time -= 50;
		
		//~ // init stoptime
		//~ stoptime = starttime + time + inc;
		
		//~ // treat increment as seconds per move when time is almost up
		//~ if (time < 1500 && inc && depth == 64) stoptime = starttime + inc - 50;
	//~ }

	//~ // if depth is not available
	//~ if(depth == -1)
		//~ // set depth to 64 plies (takes ages to complete...)
		//~ depth = 64;

	//~ // print debug info
	//~ printf("time: %d  start: %u  stop: %u  depth: %d  timeset:%d\n",
			//~ time, starttime, stoptime, depth, timeset);

	//~ // search position
	//~ search_position(depth);
//~ }



//~ // main UCI loop
//~ void UCIloop()
//~ {
	//~ // max hash MB
	//~ int max_hash = 128;
	
	//~ // default MB value
	//~ int mb = 64;

	//~ // reset STDIN & STDOUT buffers
	//~ setbuf(stdin, NULL);
	//~ setbuf(stdout, NULL);
	
	//~ // define user / GUI input buffer
	//~ char input[2000];
	
	//~ // print engine info
	//~ printf("id name HEDGINE\n");
	//~ printf("id author B.M.\n");
	//~ printf("option name Hash type spin default 64 min 4 max %d\n", max_hash);
	//~ printf("uciok\n");
	
	//~ // main loop
	//~ while (1)
	//~ {
		//~ // reset user /GUI input
		//~ memset(input, 0, sizeof(input));
		
		//~ // make sure output reaches the GUI
		//~ fflush(stdout);
		
		//~ // get user / GUI input
		//~ if (!fgets(input, 2000, stdin))
			//~ // continue the loop
			//~ continue;
		
		//~ // make sure input is available
		//~ if (input[0] == '\n')
			//~ // continue the loop
			//~ continue;
		
		//~ // parse UCI "isready" command
		//~ if (strncmp(input, "isready", 7) == 0)
		//~ {
			//~ printf("readyok\n");
			//~ continue;
		//~ }
		
		//~ // parse UCI "position" command
		//~ else if (strncmp(input, "position", 8) == 0)
		//~ {
			//~ // call parse position function
			//~ parse_position(input);
		
			//~ // clear hash table
			//~ clear_hash_table();
		//~ }
		//~ // parse UCI "ucinewgame" command
		//~ else if (strncmp(input, "ucinewgame", 10) == 0)
		//~ {
			//~ // call parse position function
			//~ parse_position("position startpos");
			
			//~ // clear hash table
			//~ clear_hash_table();
		//~ }
		//~ // parse UCI "go" command
		//~ else if (strncmp(input, "go", 2) == 0)
			//~ // call parse go function
			//~ parse_go(input);
		
		//~ // parse UCI "quit" command
		//~ else if (strncmp(input, "quit", 4) == 0)
			//~ // quit from the UCI loop (terminate program)
			//~ break;
		
		//~ // parse UCI "uci" command
		//~ else if (strncmp(input, "uci", 3) == 0)
		//~ {
			//~ // print engine info
			//~ printf("id name BBC %s\n", version);
			//~ printf("id author Code Monkey King\n");
			//~ printf("uciok\n");
		//~ }
		
		//~ else if (!strncmp(input, "setoption name Hash value ", 26)) {			
			//~ // init MB
			//~ sscanf(input,"%*s %*s %*s %*s %d", &mb);
			
			//~ // adjust MB if going beyond the aloowed bounds
			//~ if(mb < 4) mb = 4;
			//~ if(mb > max_hash) mb = max_hash;
			
			//~ // set hash table size in MB
			//~ printf("	Set hash table size to %dMB\n", mb);
			//~ init_hash_table(mb);
		//~ }
	//~ }
//~ }

// init all variables
void initializeAll(){
	// init random keys for hashing purposes
	setHashKey();
	
	//set game info
	info.quit = false;
	info.moveTime = 1;
	info.timeControl = false;
	
	
	// init hash table with default 4 MB
	allocTransTable(4);
}

// get time in milliseconds
long int getTimems(){
	
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
    char *input = NULL;  // Initialize to NULL for getLine()
    size_t bytesRead;

    // "listen" to STDIN
    if (inputWaiting()) {
		printf("joskapista\n");
		
        // Tell engine to stop calculating
        stopSearch = true;

        // Read input using getLine function
        bytesRead = getLine(&input, 1000);  // Read up to 1000 bytes -> about 200 plies
        
        // If input is available
        if (bytesRead > 0) {
            // Match UCI "quit" command
            if (!strncmp(input, "quit", 4)) {
                // Tell engine to terminate execution    
                stopSearch = true;
                info.quit = true;
            }
            // Match UCI "stop" command
            else if (!strncmp(input, "stop", 4)) {
                // Tell engine to stop searching
                stopSearch = true;
                //~ info.quit = true;
            }
        }
        
        // Free the dynamically allocated input string
        if (input != NULL) {
            free(input);
            input = NULL;
        }
    }
}


//~ // a bridge function to interact between search and GUI input
//~ void communicate() {
	//~ // if time is up break here
	//~ if(timeset == 1 && get_time_ms() > stoptime) {
		//~ // tell engine to stop calculating
		//~ stopped = 1;
	//~ }
	
	//~ // read GUI input
	//~ read_input();
//~ }


