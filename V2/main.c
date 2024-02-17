#include "game.h"
#ifdef DEBUG
	#include "debugmalloc.h"
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

//kezdeti kepet kirajzol
void printASCIIart(){
	char ASCIIart[] = "                                                                              \n                __                    ||                    __                \n               /  \\                  ====                  /  \\               \n              _\\__/_            ____  ||  ____            _\\__/_              \n              \\    /           / __ \\ /\\ / __ \\           \\    /              \n              /    \\          | /  \\ |  | /  \\ |          /    \\              \n            _/______\\_        | \\   \\ \\/ /   / |        _/______\\_            \n            \\        /         \\ \\__/ || \\__/ /         \\        /            \n           /          \\        |______________|        /          \\           \n          |____________|        \\____________/        |____________|          \n                ____                                          _               \n               / ___|___  _ __ ___  _ __ ___   __ _ _ __   __| |              \n              | |   / _ \\| '_ ` _ \\| '_ ` _ \\ / _` | '_ \\ / _` |              \n              | |__| (_) | | | | | | | | | | | (_| | | | | (_| |              \n               \\____\\___/|_| |_| |_|_|_|_|_|_|\\__,_|_| |_|\\__,_|              \n              | |   (_)_ __   ___     / ___| |__   ___  ___ ___               \n              | |   | | '_ \\ / _ \\   | |   | '_ \\ / _ \\/ __/ __|              \n              | |___| | | | |  __/   | |___| | | |  __/\\__ \\__ \\              \n              |_____|_|_| |_|\\___|    \\____|_| |_|\\___||___/___/              \n                                                                              \n                            -Press enter to start-                            \n";
	int letter = 0;
	int i = 0, j = 0;
	printf(DEFAULT);
	printf(TXT_BLACK TXT_BOLD);
	while (ASCIIart[letter] != 0){
		if ( (j % 6 < 3 ? i : i + 6) % 12 < 6){
			printf(BG_CYAN);
		}
		else{
			printf(BG_WHITE);
		}
		
		
		if (ASCIIart[letter] == '\n'){
			j++;
			i = -1;
			printf(DEFAULT TXT_BLACK TXT_BOLD);
			
		}
		printf("%c", ASCIIart[letter]);
		i++;
		letter++;
	}
	printf(DEFAULT);
}

//get_line(): egy karaktertombot tegyunk egyenlove vele, es beolvas egy sort enterig. free a hivonak! 
char *get_line(){
	int size = 1;	
	char c;
	char *array = NULL;
	if(scanf("%c", &c) == EOF){//az elejen egy sorteressel kezd 
		return array;
	}
	do {
		if (scanf("%c", &c) == EOF){
			return array;
		}
		
		if (c == '\n')
			c = 0;
		
		char *newarray = (char*) malloc(size * sizeof(char));
		if (newarray == NULL){
			printf(TXT_RED "\nMemory allocation failed!" DEFAULT);
			exit(1);
		}
		
		for (int i = 0; i < size-1; i++){
			newarray[i] = array[i];
		}
		free(array);
		array = newarray;
		
		array[size-1] = c;
		size++;
	} while (c != 0);
	return array;
}

//varakoyik, es ha nem entert kap, hanem eofot, kilep teljesen
void ruleswait(){
	char c;
	bool read = true;
	while (read && scanf("%c", &c) != EOF){
		if (c == '\n'){
			read = false;
		}
	}
	if (read){ // ha EOF volt
		exit(0);
	}
}

//interaktiv syabalzkiiras (enterrel vegig kell nzomkodni)
void rules(){
	printf("Rules of the game\n");
	ruleswait();
	printf("Movement:\nA knight moves to one of the nearest squares not on the same rank, file, or \ndiagonal. (This can be thought of as moving two squares horizontally then one\nsquare vertically, or moving one square horizontally then two squares vertically\n- i.e. in an \"L\" pattern.) The knight is not blocked by other pieces; it\njumps to the new location.<enter>\n");
	ruleswait();
	printf("The king moves exactly one square horizontally, vertically, or diagonally.\nA special move with the king known as castling is allowed only once per player, per game.<enter>\n");
	ruleswait();
	printf("A rook moves any number of vacant squares horizontally or vertically. It\nalso is moved when castling (castling however is considered\nto be a king move).<enter>\n");
	ruleswait();
	printf("A bishop moves any number of vacant squares diagonally. (Thus a bishop can\nmove to only light or dark squares, not both.)<enter>\n");
	ruleswait();
	printf("The queen moves any number of vacant squares horizontally, vertically, or\ndiagonally.<enter>\n");
	ruleswait();
	printf("A king is in check when it is under attack by at least one enemy piece.\nA piece unable to move because it would place its own king in check (it is\npinned against its own king) may still deliver check to the opposing player.\nIt is illegal to make a move that places or leaves one's king in check.<enter>\n");
	ruleswait();
	printf("Checkmate has happened, when a ckeck cannot be ceased. In this case, the player\nin check lost the game.<enter>\n");
	ruleswait();
	printf("When there are no legal moves for the side to move, and their king is not in\ncheck, the game is a stalemate (or a draw).<enter>\n");
	ruleswait();
	
	char board[12][12] = {0};
	int castling[4] = {1, 1, 1, 1};
	bool tomove;
	squarenums enpass = {-1, -1};
	int tmp = 0;
	setboardFEN("r3k3/8/8/8/1p6/8/P7/4K2R w Kq - 0 1", board, &tomove, castling, &enpass, &tmp, &tmp);
	printf(CLEAR "Castling (short - white, long - black):\n");
	printboard(board, 0, 0, tomove);
	printf("<enter>\n");
	ruleswait();
	printf(CLEAR "Castling (short - white, long - black):\n");
	move m = {{6, 2}, {8, 2}, 0};
	setmeta(board, m, castling, &enpass, &tmp);
	makemove(board, m);
	tomove = 1-tomove;
	printboard(board, 0, 0, tomove);
	printf("1.O-O\t\t  <enter>\n");
	
	ruleswait();
	printf(CLEAR "Castling (short - white, long - black):\n");
	move m2 = {{6, 9}, {4, 9}, 0};
	setmeta(board, m2, castling, &enpass, &tmp);
	makemove(board, m2);
	tomove = 1-tomove;
	printboard(board, 0, 0, tomove);
	printf("1...O-O-O\t  <enter>\n");
	ruleswait();
	
	printf(CLEAR "En passant:\n");
	move m3 = {{2, 3}, {2, 5}, 0};
	setmeta(board, m3, castling, &enpass, &tmp);
	makemove(board, m3);
	tomove = 1-tomove;
	printboard(board, 0, 0, tomove);
	printf("2.a4\t\t  <enter>\n");
	ruleswait();
	printf(CLEAR "En passant:\n");
	move m4 = {{3, 5}, {2, 4}, 0};
	setmeta(board, m4, castling, &enpass, &tmp);
	makemove(board, m4);
	tomove = 1-tomove;
	printboard(board, 0, 0, tomove);
	printf("2...bxa3\t  <enter>\n");
	ruleswait();
	printf("During the game, type -help for list of commands.\n<enter>\n");
	ruleswait();
	printf(CLEAR);
}


/* menut kiirja, meghivja a jatekot. true a return, ha nem lepunk 
 * ki a programbol, egyebkent false*/
bool menu(){ 
	printf("\n");
	for (int i = 0; i < 26; i++)
		printf("+--");
	printf("+");
	printf("\nLoad game:..............l\nGame vs CPU:............n\nTwo player game:........t\nRules:..................r\nHelp:...................h\nExit game:..............e\n\n");
	
	
	int load = 0; //0 = uj jatszma, 1 = pgn, 2 = fen
	bool read = true;
	char select[7] = {0};
	while(read && scanf("%5s", select) != EOF){
		if (strncmp(select, "l", 4) == 0 || strncmp(select, "L", 4) == 0){
			load = 1;
			read = false;
		}
		else if (strncmp(select, "t", 4) == 0 || strncmp(select, "T", 4) == 0){
			int x = game(0, 0, NULL);
			while (x == 1){
				x = game(0, 0, NULL);
			}
			if (x == 3){
				exit(0);
			}
			return true;
		}
		else if (strncmp(select, "n", 4) == 0 || strncmp(select, "N", 4) == 0){
			int x = game(0, 1, NULL);
			while (x == 1){
				x = game(0, 1, NULL);
			}
			if (x == 3){
				exit(0);
			}
			return true;
		}
		else if (strncmp(select, "e", 4) == 0 || strncmp(select, "E", 4) == 0){
			return false;
		}
		else if (strncmp(select, "r", 4) == 0 || strncmp(select, "R", 4) == 0){
			rules();
			return true;
		}
		else if (strncmp(select, "h", 4) == 0 || strncmp(select, "H", 4) == 0){
			printf("\nCommands (during the game):\nHelp:...................-h  / -help\nTake back:..............-u  / -undo\nResign:.................-re / -resign\nChange colours:.........-c  / -colour\nRotate board:...........-ro / -rotate\nSave FEN:...............-fen\nSave PGN:...............-pgn\nNew game:...............-new\nBack to main menu:......-m  / -main\nExit game:..............-exit\n\nUser input: Standard algebraic chess notation (SAN). Piece names:\nK - King\tQ - Queen\tR - Rook\tB - Bishop\tN - Knight\npawns are not notated (e.g e3).\nIf takes: 'x' after piece name (if the piece is a pawn,\nthe file it came from, than the target square (e.g. exd3).\nIf this notation is ambigous, try specifying the file, rank, or both.\nThe order is: Piece_(from file)_(from rank)_(x)_to file_to rank   e.g. Qe8xd8.\nFile input: possible from .pgn and .fen files.\nFile output: PGN (Portable Game Notation), FEN (Forsyth-Edwards Notation).\n      Command Line Chess\n2023 Blahota Marcell: nagy hazi\n<enter>");
			ruleswait();
			ruleswait();
			return true;
		}
		else{
			printf("Unknown command!\n");
		}
	}
	if (read){
		return false; //ide akkor jut el, ha EOF volt
	}
	
	read = true;
	printf("\nLoad PGN:...............pgn\nLoad FEN:...............fen\nExit game:..............e\n\n");
	char *filename;

	while(read && scanf("%5s", select) != EOF){
		if (strncmp(select, "fen", 4) == 0 || strncmp(select, "FEN", 4) == 0){
			if (system("ls *.fen") != 0){
				return false;
			}
			printf("\nFilename (example.fen): ");
			
			filename = get_line();
			if (filename == NULL){
				printf(TXT_RED "Unable to load FEN!\n" DEFAULT);
				return true;
			}
			if (filename[0] == 0){
				printf(TXT_RED "Unable to load FEN!\n" DEFAULT);
				free(filename);
				return true;
			}
			
			load = 2;
			read = false;
		}
		else if (strncmp(select, "pgn", 4) == 0 || strncmp(select, "PGN", 4) == 0){
			if (system("ls *.pgn") != 0){
				return false;
			}
			printf("\nFilename (example.pgn): ");
			
			filename = get_line();
			if (filename == NULL){
				printf(TXT_RED "Unable to load PGN!\n" DEFAULT);
				return true;
			}
			if (filename[0] == 0){
				printf(TXT_RED "Unable to load PGN!\n" DEFAULT);
				free(filename);
				return true;
			}
			load = 1;
			read = false;
		}
		else if (strncmp(select, "e", 5) == 0 || strncmp(select, "E", 5) == 0){
			return false;
		}
		else{
			printf("Unknown command!\n");
		}
	}
	if (read){
		return false; //ide akkor jut el, ha EOF volt
	}
	read = true;
	//filename scanf
	
	printf("Game vs CPU:............n\nTwo player game:........t\nExit game:..............e\n\n");
	while(scanf("%5s", select) != EOF){
		if (strncmp(select, "t", 4) == 0 || strncmp(select, "T", 4) == 0){
			int x = game(load, 0, filename);
			while (x == 1){
				x = game(load, 0, filename);
			}
			if (x == 3){
				free(filename);
				exit(0);
			}
			free(filename);
			return true;
		}
		else if (strncmp(select, "n", 4) == 0 || strncmp(select, "N", 4) == 0){
			int x = game(load, 1, filename);
			while (x == 1){
				x = game(load, 1, filename);
			}
			if (x == 3){
				free(filename);
				exit(0);
			}
			free(filename);
			return true;
		}
		else if (strncmp(select, "e", 5) == 0 || strncmp(select, "E", 5) == 0){
			free(filename);
			return false;
		}
		else{
			printf("Unknown command!\n");
		}
	}
	if (read){
		free(filename);
		return false; //ide akkor jut el, ha EOF volt
	}
	free(filename);
	return true;
}

int main(){
	printf(DEFAULT CLEAR);
	printASCIIart();
	char c;
	while(c != '\n'){
		if (scanf("%c", &c) == EOF){
			return 0;
		}
	}
	printf(CLEAR);
	bool run = true;
	while (run) {
		run = menu();
	}
	
	return 0;
}



