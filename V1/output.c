#include "output.h"

int boardvalue(char board[12][12]){
	int n = 0;
	for (int i = 9; i > 1; i--){
		for (int j = 2; j < 10; j++){
			switch(board[i][j]){
				case 'p': 
					n -= 1;
					break;
				case 'P': 
					n += 1;
					break;
				case 'n': 
					n -= 3;
					break;
				case 'N': 
					n += 3;
					break;
				case 'b': 
					n -= 3;
					break;
				case 'B': 
					n += 3;
					break;
				case 'r': 
					n -= 5;
					break;
				case 'R': 
					n += 5;
					break;
				case 'q': 
					n -= 9;
					break;
				case 'Q': 
					n += 9;
			}
		}
	}
	return n;
	
}

/* kiirja a tablat egyszeru szovegkent, egyszeru karakterekkel,
 * ha a megjelenites alapbol rossz, ez valoszinuleg mukodik.*/
void printboard_letters(char board[12][12]){
	printf("|");
	for (int j = 2; j < 9; j++){
		printf("---+");
	}
		printf("---|\n");
	for (int i = 9; i > 1; i--){
		printf("|");
		for (int j = 2; j < 10; j++){
			printf(" %c |", board[i][j]);
		}
		printf("\n|");
		for (int j = 2; j < 9; j++){
			printf("---+");
		}
		printf("---|\n");
	}
	printf("\n");
}

/* kirajzol egy fekete-feher tablat, megadott szemkozbol, 
 * jelezve hogy ki lep.*/
void printblackandwhite(char board[12][12], int view, bool tomove) { 
	printf(TXT_BOLD TXT_BLACK BG_WHITE "%c", (tomove ? 'b' : 'w'));
	printf(DEFAULT " ");
	for (char c = 'a' + (view ? 7 : 0); c != 'i' - (view ? 9 : 0) ; c += (view ? -1 : 1)){
		printf("%c ", c);
	}
	printf(" ");
	if (boardvalue(board) >= 0)	printf(TXT_BLACK BG_WHITE);
	else printf(TXT_WHITE BG_BLACK);
	
	printf(" %c%d", (boardvalue(board) > 0 ? '+' : (boardvalue(board) == 0 ? ' ' : 0)), boardvalue(board));
	printf(DEFAULT "\n");	
	
	for(int i = (view ? 2 : 9); i != (view ? 10 : 1); i += (view ? 1 : -1)){
		printf("%d ", i-1);
		for(int j = (view ? 9 : 2); j != (view ? 1 : 10); j += (view ? -1 : 1)){
			if ((i + j) % 2){
				printf(TXT_BLACK BG_WHITE);
			}
			else{
				printf(TXT_WHITE BG_BLACK);
			}
			//printf(" ");
			switch(board[i][j]){
				case 'p': 
					if ((i+j) % 2 == 1) printf("♟︎"); 
					else  printf("♙"); 
					break;
				case 'P': 
					if ((i+j) % 2 == 0) printf("♟︎"); 
					else  printf("♙"); 
					break;
				
				case 'n': 
					if ((i+j) % 2 == 1) printf("♞"); 
					else  printf("♘"); 
					break;
				case 'N': 
					if ((i+j) % 2 == 0) printf("♞"); 
					else  printf("♘"); 
					break;
				case 'b': 
					if ((i+j) % 2 == 1) printf("♝"); 
					else  printf("♗"); 
					break;
				case 'B': 
					if ((i+j) % 2 == 0) printf("♝"); 
					else  printf("♗"); 
					break;
				case 'r': 
					if ((i+j) % 2 == 1) printf("♜"); 
					else  printf("♖"); 
					break;
				case 'R': 
					if ((i+j) % 2 == 0) printf("♜"); 
					else  printf("♖"); 
					break;
				case 'q': 
					if ((i+j) % 2 == 1) printf("♛"); 
					else  printf("♕"); 
					break;
				case 'Q': 
					if ((i+j) % 2 == 0) printf("♛"); 
					else  printf("♕"); 
					break;
				case 'k': 
					if ((i+j) % 2 == 1) printf("♚"); 
					else  printf("♔"); 
					break;
				case 'K': 
					if ((i+j) % 2 == 0) printf("♚"); 
					else  printf("♔"); 
					break;
				default: printf(" ");
			}
			printf(" ");
		}
		printf(DEFAULT);
		printf("\n");
	}
	printf(DEFAULT);
}

/* kirajzol egy tablat adott szemszogbol, jelezve ki lep.
 * Valaszthato a szinsema, ha fekete feher, vagy betuk,
 * akkor a korabbi fv-eket hivja */
void printboard(char board[12][12], int view, int col, bool tomove) { //feher szemszog = 0
	//♔♚♕♛♖♜♗♝♘♞♙♟︎
	if (col % 8 == 7){
		printblackandwhite(board, view, tomove);
		return;
	}
	if (col % 8 == 5){
		printboard_letters(board);
		return;
	}
	
	char *colour[] = {BG_YELLOW, BG_RED, BG_GREEN, BG_BLUE, BG_MAGENTA, 0, BG_CYAN, BG_BLACK}; //8 tablaszin
	
	printf(TXT_BOLD TXT_BLACK BG_WHITE "%c", (tomove ? 'b' : 'w'));
	printf(DEFAULT " ");
	for (char c = 'a' + (view ? 7 : 0); c != 'i' - (view ? 9 : 0) ; c += (view ? -1 : 1)){
		printf("%c ", c);
	}
	printf(" ");
	if (boardvalue(board) >= 0)	printf(TXT_BLACK BG_WHITE);
	else printf(TXT_WHITE BG_BLACK);
	
	printf("%c%d", (boardvalue(board) > 0 ? '+' : (boardvalue(board) == 0 ? ' ' : 0)), boardvalue(board));
	printf(DEFAULT "\n");
	for(int i = (view ? 2 : 9); i != (view ? 10 : 1); i += (view ? 1 : -1)){
		printf("%d ", i-1);
		printf(TXT_BLACK);
		for(int j = (view ? 9 : 2); j != (view ? 1 : 10); j += (view ? -1 : 1)){
			if ((i + j) % 2)
				printf(BG_WHITE);
			else
				printf("%s", colour[col % 8]);
			//printf(" ");
			switch(board[i][j]){
				case 'p': printf("♟︎"); break;
				case 'P': printf("♙"); break;
				case 'n': printf("♞"); break;
				case 'N': printf("♘"); break;
				case 'b': printf("♝"); break;
				case 'B': printf("♗"); break;
				case 'r': printf("♜"); break;
				case 'R': printf("♖"); break;
				case 'q': printf("♛"); break;
				case 'Q': printf("♕"); break;
				case 'k': printf("♚"); break;
				case 'K': printf("♔"); break;
				default: printf(" ");
			}
			printf(" ");
		}
		printf(DEFAULT);
		printf("\n");
	}
	printf(DEFAULT);
}
