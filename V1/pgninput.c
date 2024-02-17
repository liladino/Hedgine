#include "pgninput.h"

/* beolvassa a file-t, es a tartalmat visszaadja dinamikusan 
 * foglalt stringkent, amit a hivo baja felszabaditani.*/
char *readPGN(char *filename){
	FILE *fptr;
	if ((fptr = fopen(filename,"r")) == NULL){
		printf(TXT_RED "Unable to load PGN\n" TXT_WHITE);
		return NULL;
	}
	int size = 1;	
	char c;
	char *array = NULL;
	do {
		if (fscanf(fptr, "%c", &c) == EOF){
			c = 0;
		}
		char *newarray = (char*) malloc(size * sizeof(char));
		if (newarray == NULL){
			printf(TXT_RED "Memory allocation failed\n" TXT_WHITE);
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
	
	fclose(fptr);
	return array;
}

/* a fajlbol keszult stringbol jatszmat csinal, azaz beirja
 * a lepeseket a listaba, fellepest, lepest, 50 lepeses szabalyt 
 * szamol, sancot, en passant-ot beallitja. */
int makegame(char PGN[], movelist *game, char board[12][12], bool *tomove, int *halfmove, int *movenum, int *fmv, int castling[4], squarenums *enpass){
	int i = 0;
	bool ongoing = false;
	
	while(PGN[i] != 0 && !ongoing){
		if (PGN[i] == '[' && PGN[i + 1] == 'R'){
			char str[10] = {0};
			str[0] = 'R';
			for (int j = 1; j < 8 && ((PGN[i + j + 1] <= 'z' && PGN[i + j + 1] >= 'a') || PGN[i + j + 1] == ' ' || PGN[i + j + 1] == '"'); j++){
				str[j] = PGN[i + j + 1];
			}
			if (strcmp(str, "Result \"") == 0){
				if (PGN[i + 9] != '*'){
					printf("[Result \"%c%c%c%c]\n", PGN[i+9], PGN[i+10],PGN[i+11],PGN[i+12]);
					printf("* not found, game is not ongoing\n");
					free(PGN);
					return 1;
				}
				else{
					printf("[%s*\"]\n", str);
					ongoing = true;
					break;
				}
				
			}
		}
		printf("%c", PGN[i]);
		i++;
	}
	if (!ongoing){ //vegigment a fajlon, es nem talalt [Result "* karaktersort
		free(PGN);
		return 1;
	}
	while(PGN[i] != 0 && PGN[i] != ']'){
		i++;
	}
	if (PGN[i] == 0){
		free(PGN);
		return 1;
	}
	

	if (setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board, tomove, castling, enpass, halfmove, movenum)){
		return 1;
	}
	
	while(PGN[i] != 0){
		//char debug = PGN[i];
		if ((PGN[i] >= '0' && PGN[i] <= '9') || PGN[i] == '.' || PGN[i] == '}' || PGN[i] == '\n' || PGN[i] == ')' || PGN[i] == ']'){
			i++;
			continue;
		}
		if (PGN[i] == '{'){ //comment, nem halmozodhatnak
			while (PGN[i] != 0 && PGN[i] != '}'){
				i++;
			}
			continue;
		}
		if (PGN[i] == ';' || PGN[i] == '%'){ //comment a sor vegeig
			while (PGN[i] != 0 && PGN[i] != '\n'){
				i++;
			}
			continue;
		}
		if (PGN[i] == '('){ //mellekvonalak, halmozodhatnka
			int count = 0;
			do {
				if (PGN[i] == '('){
					count++;
				}
				else if (PGN[i] == ')'){
					count--;
				}
				i++;
			} while (PGN[i] != 0 && count > 0);
			if (PGN[i] == 0){
				free(PGN);
				freemoves(game);
				return 1;
			}
			continue;
		}
		if (PGN[i] == '['){ //egyeb jelzesek
			int count = 0;
			do{
				if (PGN[i] == '['){
					count++;
				}
				else if (PGN[i] == ']'){
					count--;
				}
				i++;
			} while (PGN[i] != 0 && count > 0);
			if (PGN[i] == 0){
				free(PGN);
				freemoves(game);
				return 1;
			}
			continue;
		}
		if (PGN[i] == '#' || PGN[i] == '/'){
			free(PGN);
			freemoves(game);
			return 1;
		}
		if (PGN[i] == '*'){
			printf("*\n");
			break;
		}
		int j = 0;
		char SAN_move[10] = {0};
		while (j < 9 && ((PGN[i] >= 'a' && PGN[i] <= 'z') || (PGN[i] >= 'A' && PGN[i] <= 'Z') || (PGN[i] >= '0' && PGN[i] <= '9') || PGN[i] == '-' || PGN[i] == '=' || PGN[i] == '+')){
			//a lepesben betuk, szamok, + es = lehet
			SAN_move[j] = PGN[i];
			i++;
			j++;
		}
		i++;
		if (SAN_move[0] != 0){
			if (*tomove)
				printf("%s ", SAN_move);
			else
				printf("%d.%s ", *movenum, SAN_move);
			
			squarenums start = {-1, -1};
			move m = initializemove(start, start, 0);
			movelist *legalmoves = createMove(m);
			
			generatemoves(board, legalmoves, *tomove, castling, *enpass);
			int convertreturn = fromSANconvert(SAN_move, &m, board, *tomove, legalmoves);
			
			if (convertreturn != 0) {
				printf("Invalid input! (%d)\n", convertreturn);
				printboard(board, 0, 6, tomove);
				//freemoves(game); //hivo felszabaditja!
				free(PGN);
				freemoves(legalmoves);
				return 1;
			}
			
			if (!checkiflegal(m, legalmoves)){
				printf("Illegal move!\n");
				//freemoves(game);
				free(PGN);
				freemoves(legalmoves);
				return 1;
			}
			
			insertMove(game, m);
		
			setmeta(board, m, castling, enpass, fmv);
			makemove(board, m);
			*tomove = 1 - *tomove;
			(*halfmove)++;
			if (*tomove){
				(*movenum)++;
			}
			freemoves(legalmoves);
			//printf("%c%d %c%d %c %c ep: %c%d\n", m.from.file - 2 + 'a', m.from.rank-1, m.to.file - 2 + 'a', m.to.rank-1, m.promotion, incheck(board, !tomove) == 0 ? ' ' : '+', (enpass->file == -1 ? 0 : enpass->file + 'a' - 2), (enpass->rank == -1 ? -1 : enpass->rank - 1)); //debug
			if (*fmv == 100){
				freemoves(game);
				free(PGN);
				return 1;
			}
		}
		
	}
	//printboard(board, 0, 6);
	free(PGN);
	printf("\n");
	return 0;
}

/* a kapott filenevvel beolvassa a PGN-t, beallitja a tablat, letrehozza a 
 * jatszmat es beirja az eddig megtett lepeseket, beallitja az aktualis
 * sancolasi es en passant lehetosegeket, a fellepesszamot, a lepesszamot,
 * es az 50 lepeses szabaly szamlalot*/
int loadPGN(char *filename, char board[12][12], movelist *game, bool *tomove, int castling[], squarenums *enpass, int *halfmove, int *movenum, int *fmv){
	char* PGN = readPGN(filename);
	if (PGN == NULL){
		return 1;
	}
	
	if (makegame(PGN, game, board, tomove, halfmove, movenum, fmv, castling, enpass) != 0){
		printf(TXT_RED "Couldn't load PGN!\n" TXT_WHITE);
		return 1;
	}
	//freemoves(game);
	return 0;
}

