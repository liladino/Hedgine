#include "game.h"


//kerdezi a felhasznalot, hogy akarja-e, hogy elmentse a meccset pgnbe (a parti befejezte utan)
void save(char *loadfile, movelist *game, int playercol, int result){
	printf("Do you wish to save the game (y/n)? ");
	char c;
	bool read = true;
	while (scanf("%c", &c) != EOF && read){
		switch (c){
			case 'y':
				if (savepgn(loadfile, game, playercol, result))
					printf("Couldn't save PGN!\n");
				else
					printf("PGN saved!\n");
				read = false;
				break;
			case 'n':
				read = false;
				break;
		}
	}
}

//az undo utan a haromszori lepesismetles felteteleit visszaallitja alapallasukba (egyebkent a legutolso utesig vagy gyaloglepesig eleg nezni)
void repetition_reset(char s_board[12][12], bool *s_tomove, int s_castling[4], squarenums *s_enpass, char *loadfile){
	int temp1 = 0, temp2 = 0;
	if (loadfile != NULL){
		loadFEN(loadfile, s_board, s_tomove, s_castling, s_enpass, &temp1, &temp2);
	}
	else{
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", s_board, s_tomove, s_castling, s_enpass, &temp1, &temp2);
	}
}

//a jatekot futtatja, 0 ha lefutott, 1 ha uj jatek kell, 3 ha kilepunk a programbol
int game(int load, int players, char *lf_orig){ //load: 0 = uj jatszma, 1 = pgn, 2 = fen     players: 0: 2 jatekos 1: cpu ellen 
	setHashKey();
	printf(CLEAR "\n");
	char *loadfile; //ennek adhat NULL erteket, es az eredeti pointert pedig lehet free-elni
	loadfile = lf_orig;
	char board[12][12] = {{0}};
	int view = 0;
	int playercol = 0;
	int cpulvl = -1;
	if (players == 1){//1 a cpu
		printf("CPU level (0-2)? ");
		char c;
		if (scanf("%c", &c) == EOF){
			return 0;
		}
		bool read = true;
		while (read && scanf("%c", &c) != EOF){
			cpulvl = c - '0';
			if (cpulvl >= 0 && cpulvl <= 2){
				read = false;
			}
		}
		if (read){ // ha EOF volt
			return 0;
		}
		
		printf("Player colour (w/b)? ");
		
		
		read = true;
		while (read && scanf("%c", &c) != EOF ){
			switch (c){
				case 'w':
					playercol = 0;
					read = false;
					break;
				case 'b':
					playercol = 1;
					read = false;
					break;
			}
		}
		if (read){ // ha EOF volt
			return 0;
		}
	}
	else{
		playercol = 2;
	}
	
	bool tomove = 0; //0 -> feher; 1 -> fekete 
	int halfmove = 0, movenum = 1, fmv = 0; //fifty-move rule
	int boardcol = 6;
	int castling[4];
	squarenums enpass = {-1, -1};
	squarenums start = {-1, -1};
	move m = initializemove(start, start, 0);
	movelist *game = createMove(m);
	
	
	char s_board[12][12];
	movelist *repet_start;
	repet_start = game;
	bool s_tomove;
	int s_castling[4];
	squarenums s_enpass;
	
	if (load == 2){
		if (loadFEN(loadfile, board, &tomove, castling, &enpass, &halfmove, &movenum) == 1){
			freemoves(game);
			return 0;
		}
		if (incheck(board, 1-tomove) == 1){
			printf(TXT_RED "Unable to load FEN!\n"  TXT_WHITE);
			freemoves(game);
			return 0;
		}
		int temp1 = 0, temp2 = 0;
		loadFEN(loadfile, s_board, &s_tomove, s_castling, &s_enpass, &temp1, &temp2);
	}
	else {
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, &tomove, castling, &enpass, &fmv, &movenum);
		int temp1 = 0, temp2 = 0;
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", s_board, &s_tomove, s_castling, &s_enpass, &temp1, &temp2);
		if (load == 1){
			if (loadPGN(loadfile, board, game, &tomove, castling, &enpass, &halfmove, &movenum, &fmv) == 1){
				freemoves(game);
				return 0;
			}
			loadfile = NULL;
		}
	}
	
	if (playercol % 2){
		view = 1-view;
	}
	
	printboard(board, view, boardcol, tomove);
	/*#ifdef DEBUG
	makeBenchTest(board, tomove, castling, enpass);
	#endif*/
	while(1){
		printf(DEFAULT); 

		if (fmv == 100){ //100ig, mert 50 egesz lepes
			printf("Draw by 50 move rule!\n");
			save(loadfile, game, playercol, 6);
			freemoves(game);
			return 0;
		}
		int matingforce = checkfordraw(board);
		if (matingforce == 1){
			printf("Draw (insufficient material)!\n");
			save(loadfile, game, playercol, 6);
			freemoves(game);
			return 0;
		}
		else if (matingforce == 2){
			printf("Illegal board!\n");
			freemoves(game);
			return 0;
		}
		if (repetition(s_board, repet_start, s_tomove, s_castling, s_enpass, board, tomove, castling, enpass)){
			printf("Draw (threefold repetition)!\n");
			save(loadfile, game, playercol, 6);
			freemoves(game);
			return 0;
		}
		
		
		move temp = initializemove(start, start, 0);
		movelist *legalmoves = createMove(temp);
		
		generatemoves(board, legalmoves, tomove, castling, enpass);
		if (legalmoves->next == NULL){
			int result;
			if (incheck(board, tomove) == 0){
				result = 3;
				printf("Stalemate!\n");
			}
			else if (tomove){
				result = 1;
				printf("White won!\n");
			}
			else {
				result = 2;
				printf("Black won!\n");
			}
			freemoves(legalmoves);
			
			save(loadfile, game, playercol, result);
			
			break;
		}
		if (incheck(board, tomove) == 1){
			printf("Check!  ");
		}
		//printmoves(legalmoves);
		//printf("eval: %f\n", basic_eval(board));
		
		if (players == 0 || (players == 1 && playercol == tomove)){ 
		//if ( (players == 1 && playercol == tomove)){//debug: ket gep lepked egymas ellen, ha two players
			int playermovereturn = playermove(board, loadfile, game, legalmoves, &m, &tomove, castling, &enpass, &halfmove, &movenum, &fmv, &view, &boardcol, playercol);
			if (playermovereturn == 0){
				printf(CLEAR "\n");
			}
			else if (playermovereturn == 1){
				freemoves(legalmoves);
				continue;
			}
			else if (playermovereturn == 2){
				freemoves(legalmoves);
				save(loadfile, game, playercol, (!tomove ? 5 : 4));
				break;
			}
			else if (playermovereturn == 3){ //exit
				freemoves(legalmoves);
				freemoves(game);
				return 3;
			}
			else if (playermovereturn == 4){ //new
				freemoves(legalmoves);
				freemoves(game);
				return 1;
			}
			else if (playermovereturn == 5){ //undo, ilyenkor a haromszori lepesismetles visszaallitasa
				printf(CLEAR "\n");
				if (players == 0){ //aut tablaforg ne romoljon el
					view = 1-view;
				}
				
				freemoves(legalmoves);
				repet_start = game;
				repetition_reset(s_board, &s_tomove, s_castling, &s_enpass, loadfile);
				
				printboard(board, view, boardcol, tomove);
				continue;
			}
			else if (playermovereturn == 6){ //menu
				freemoves(legalmoves);
				break;
			}
		}
		else{
			printf("\n");
			m = CPU(cpulvl, board, tomove, castling, enpass);
			if ((board[m.from.rank][m.from.file] == 'K' || board[m.from.rank][m.from.file] == 'k') && abs(m.from.file - m.to.file) == 2){
				printf(CLEAR "O-O");
				if (m.from.file - m.to.file == 2){
					printf("-O");
				}
				printf("\n\a");
			}
			else{
				char enginemove[10] = {0};
				convert_back(board, m, enginemove);
				printf(CLEAR);
				printf("%s\n\a", enginemove);
			}
		}
		
		
		if (players == 0){ //ket jatekosnal automatikus tablaforgatas a kovetkezo felnek
			view = 1-view;
		}
		
		insertMove(game, m);
		
		
		setmeta(board, m, castling, &enpass, &fmv);
		if (board[m.from.rank][m.from.file] == 'p' || board[m.from.rank][m.from.file] == 'P' || board[m.to.rank][m.to.file] != ' '){
			//ha utes vagy gyaloglepes, a 3szori lepesismetles kezdoallasa lehet ez
			repet_start = game;
			while (repet_start->next != NULL){
				repet_start = repet_start->next;
			}
			
			copyBoard(board, s_board);
			s_castling[0] = castling[0]; s_castling[1] = castling[1]; s_castling[2] = castling[2]; s_castling[3] = castling[3]; 
			s_enpass.file = enpass.file; s_enpass.rank = enpass.rank;
			s_tomove = !tomove;
		}
		makemove(board, m);
		
		//printf("%c%d %c%d %c %c ep: %c%d fmv: %d\n", m.from.file - 2 + 'a', m.from.rank-1, m.to.file - 2 + 'a', m.to.rank-1, m.promotion, incheck(board, !tomove) == 0 ? ' ' : '+', (enpass.file == -1 ? 0 : enpass.file + 'a' - 2), (enpass.rank == -1 ? -1 : enpass.rank - 1), fmv); //debug
		
		

		if (tomove){
			movenum++;
		}
		tomove = 1-tomove;
		halfmove++;
		
		
		printboard(board, view, boardcol, tomove);
		
		freemoves(legalmoves);
		
	}
	freemoves(game);
	//printf(CLEAR);
	return 0;
}

/* beker egy lepest vagy commandot a felhasznalotol, ebbol lepest csinal (ha command, akkor megfelelo visszateresi ertekkel kilep)
 * ha nem lehetseges lepest, vagy ismeretlen commandot kapott, nem 0 a visszateresi ertek, ha minden jo volt, 0.
 * */ 
int playermove(char board[12][12], char *loadfile, movelist *game, movelist *legalmoves, move *m, bool *tomove, int castling[], squarenums *enpass, int *halfmove, int *movenum, int *fmv, int *view, int *boardcol, int playercol){
	
	char SAN_move[10] = {0}; //Standard Algebraic Notation, leghosszabbak: Qa1xe4+, fxg8=Q+ -> 7 char
	if (scanf("%9s", SAN_move) == EOF){
		return 2;
	}
	if (SAN_move[0] == '-'){
		if (strcmp("-resign", SAN_move) == 0 || strcmp("-re", SAN_move) == 0){
			printf("%s lost!\n", (*tomove ? "Black" : "White"));
			return 2;
		}
		else if ((strcmp("-undo", SAN_move) == 0 || strcmp("-u", SAN_move) == 0) ){
			if (game->next != NULL){ // a startjelzo lepes nem mutat sehova
				if (playercol < 2 && game->next->next == NULL){
					//ha gep ellen jatszunk, es vissza akarunk lepni, kettot kell. ha ez nem lehet, akkor continue
					printf("No moves made yet!\n");
					return 1;
				}
				
				undoMove(&game);
				if (playercol < 2){
					undoMove(&game);
				}
				if (loadfile != NULL){
					bool temp1;
					if (loadFEN(loadfile, board, &temp1, castling, enpass, fmv, movenum) == 1){
						freemoves(game);
						return 2;
					}
				}
				else{
					resetBoard(board);
					castling[0] = 1; castling[1] = 1; castling[2] = 1; castling[3] = 1; 
					enpass->file = -1; enpass->rank = -1;
					if (!(*tomove)){
						(*movenum)--;
					}
				}
				playmoves(board, game, castling, enpass, fmv);
				
				*tomove = 1 - *tomove;
				(*halfmove)--;
				if (playercol < 2){
					*tomove = 1 - *tomove;
					(*halfmove)--;
				}
				
				return 5;
			}
			else {
				printf("No moves made yet!\n");
				return 1;
			}
			
		}
		#ifdef DEBUG
		else if (strcmp("-reset", SAN_move) == 0){
			if (loadfile != NULL){
				bool temp1;
				if (loadFEN(loadfile, board, &temp1, castling, enpass, fmv, movenum) == 1){
					freemoves(game);
					return 2;
				}
			}
			else{
				resetBoard(board);
				castling[0] = 1; castling[1] = 1; castling[2] = 1; castling[3] = 1; 
				enpass->file = -1; enpass->rank = -1;
				if (!(*tomove)){
					(*movenum)--;
				}
			}
			return 1;
		}
		#endif
		else if (strcmp("-colour", SAN_move) == 0 || strcmp("-c", SAN_move) == 0){
			(*boardcol)++;
			printboard(board, *view, *boardcol, tomove);
			return 1;
		}
		else if (strcmp("-print", SAN_move) == 0){
			printmoves(game);
			return 1;
		}
		else if (strcmp("-rotate", SAN_move) == 0 || strcmp("-ro", SAN_move) == 0){
			*view = 1 - *view;
			printboard(board, *view, *boardcol, tomove);
			return 1;
		}
		else if (strcmp("-fen", SAN_move) == 0){
			if (savefen(board, *tomove, castling, *enpass, *fmv, *movenum)){
				printf(TXT_RED "Couldn't save FEN!\n" TXT_WHITE);
				return 2;
			}
			printf("saved!\n");
			return 1;
		}
		else if (strcmp("-pgn", SAN_move) == 0){
			if (savepgn(loadfile, game, playercol, 0)){
				printf("Couldn't save PGN!\n");
				return 2;
			}
			printf("saved!\n");
			return 1;
		}
		else if (strcmp("-perf", SAN_move) == 0){
			makeBenchTest(board, !tomove, castling, *enpass);
			return 1;
		}
		else if (strcmp("-exit", SAN_move) == 0){
			return 3;
		}
		else if (strcmp("-new", SAN_move) == 0){
			return 4;
		}
		else if (strcmp("-m", SAN_move) == 0 || strcmp("-main", SAN_move) == 0){
			return 6;
		}
		else if (strcmp("-help", SAN_move) == 0 || strcmp("-h", SAN_move) == 0){
		printf("\nCommands:\nHelp:...................-h  / -help\nTake back:..............-u  / -undo\nResign:.................-re / -resign\nChange colours:.........-c  / -colour\nRotate board:...........-ro / -rotate\nSave FEN:...............-fen\nSave PGN:...............-pgn\nNew game:...............-new\nBack to main menu:......-m  / -main\nPerformance test:.......-perf\nExit game:..............-exit\n\nUser input: Standard algebraic chess notation (SAN). Piece names:\nK - King\tQ - Queen\tR - Rook\tB - Bishop\tN - Knight\npawns are not notated (e.g e3).\nIf takes: 'x' after piece name (if the piece is a pawn,\nthe file it came from, than the target square (e.g. exd3).\nIf this notation is ambigous, try specifying the file, rank, or both.\nThe order is: Piece_(from file)_(from rank)_(x)_to file_to rank   e.g. Qe8xd8.\nFile input: possible from .pgn and .fen files.\nFile output: PGN (Portable Game Notation), FEN (Forsyth-Edwards Notation).\n\n      Command Line Chess\n2023 Blahota Marcell: nagy hazi\n");
			return 1;
		}
		else {
			printf("Unknown command! (Try -help)\n");
			return 1;
		}
	}
	
	
	int convertreturn = fromSANconvert(SAN_move, m, board, *tomove, legalmoves);
	switch (convertreturn) {
		case 1:
			printf("Invalid move!\n");
			return 1;
		case 2:
			printf("Ambigous notation!\n");
			return 1;
		case 3:
			printf("Illegal move!\n");
			return 1;
	}
	
	
	return 0;	
}

/* lejatssza a kapott lepeslistat a kapott kezdoallastol, es beallitja a sancolasi en passant lehetoseg(ek)et, illetve az otven lepeses szabaly szamlalojat
 * */
void playmoves(char board[12][12], movelist *head, int castling[], squarenums *enpass, int *fmv) {
	movelist *current = head;
	while (current != NULL) {
		if (current->m.from.rank != -1){
			setmeta(board, current->m, castling, enpass, fmv);
			makemove(board, current->m);
		}
		current = current->next;
	}
}

