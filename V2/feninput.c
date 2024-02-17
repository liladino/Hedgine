#include "feninput.h"

/* beallitja a tablat es metaadatokat FEN alapjan*/
int setboardFEN(char FEN[], char board[12][12], bool *tomove, int castling[], squarenums *enpass, int *fmv, int *movenum){ 
	//startallas: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (i < 2 || i > 9 || j < 2 || j > 9)
				board[i][j] = 0;
		}
	}
	
	int rank = 9, file = 2;
	unsigned int i = 0;
	bool boardread = true;
	while (boardread){
		if (FEN[i] == '/'){
			rank--;
			file = 1; //a ciklus vegen novekszik eggyel mindenkepp
		}
		else if (FEN[i] >= '1' && FEN[i] <= '8'){
			for (int j = 0; j < FEN[i] - '0'; j++){
				if (file > 9) {
					return 1;
				}
				board[rank][file] = ' ';
			//	printboard_letters(board);
				file++;
			}
			file--;
		}
		else if (FEN[i] == 'r' || FEN[i] == 'R' || FEN[i] == 'n' || FEN[i] == 'N' || FEN[i] == 'b' || FEN[i] == 'B' || FEN[i] == 'q' || FEN[i] == 'Q' || FEN[i] == 'k' || FEN[i] == 'K' || FEN[i] == 'p' || FEN[i] == 'P' ){
			if (file > 9){
				return 1;
			}
			board[rank][file] = FEN[i];
		//	printboard_letters(board);
		}
		else if (FEN[i] == ' '){
			boardread = false;
		}
		else{
			return 1;
		}
		i++;
		file++;
	}
	int wking = 0, bking = 0;
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (board[i][j] == 'K')
				wking++;
			if (board[i][j] == 'k')
				bking++;
		}
	}
	if (wking != 1 || bking != 1){
		return 1;
	}
	
	
	//i++;
	char* metadata = (char*)malloc( (strlen(FEN) + 1) * sizeof(char));
	for (unsigned int x = 0; x < strlen(FEN); x++){
		metadata[x] = 0;
	}
	
	int j = 0;
	int spacecount = 0;
	while(FEN[i] != 0){
		metadata[j] = FEN[i];
		if (metadata[j] != ' '){
			if (spacecount == 5){ // a leghosszabb str benne KQkq, ami 4 char, ha ennel hosszabb, az sscanf hibas
				free(metadata);				
				return 1;
			}
			spacecount++;
		}
		else{
			spacecount = 0;
		}
		i++; 
		j++;
	}
	
	char tomove_char;
	char castle_str[5] = {0};
	char enpass_str[5] = {0};
	if (sscanf(metadata, "%c %4s %2s", &tomove_char, castle_str, enpass_str) != 3){
		free(metadata);	
		return 1;
	}
	
	if (tomove_char == 'w'){
		*tomove = 0;
	}
	else if (tomove_char == 'b'){
		*tomove = 1;
	}
	else{
		free(metadata);	
		return 1;
	}
	
	for (int x = 0; x < 4; x++){
		castling[x] = 0;
	}
	i = 0;
	if (castle_str[0] != '-'){
		while (castle_str[i] != 0){
			switch (castle_str[i]){
				case 'K':
					castling[0] = 1;
					break;
				case 'Q':
					castling[1] = 1;
					break;
				case 'k':
					castling[2] = 1;
					break;
				case 'q':
					castling[3] = 1;
					break;
				default:
					free(metadata);	
					return 1;
			}
			i++;
		}
	}

	if (enpass_str[0] == '-'){
		enpass->file = -1;
		enpass->rank = -1;
	}
	else{
		if (enpass_str[0] >= 'a' && enpass_str[0] <= 'h'){
			enpass->file = enpass_str[0] - 'a' + 2;
		}
		else{
			free(metadata);	
			return 1;
		}
		if (enpass_str[1] == '3' || enpass_str[1] == '6'){
			enpass->rank = enpass_str[1] - '0' + 1;
		}
		else{
			free(metadata);	
			return 1;
		}
	}
	
	if (sscanf(metadata, " %d %d", fmv, movenum) != 2){
		*fmv = 0;
		*movenum = 0; 
	}
	free(metadata);
	return 0;
}

/* beolvas egy file-bol egy FEN-t, es beteszi a tartalmat egy stringbe*/
char *readFEN(char *filename){
	FILE *fptr;
	if ((fptr = fopen(filename,"r")) == NULL){
		printf(TXT_RED "Unable to load FEN\n" DEFAULT);
		return NULL;
	}
	int size = 1;	
	char c;
	char *array = NULL;
	do {
		if (fscanf(fptr, "%c", &c) == EOF){
			c = 0;
		}
		else if (c == '\n'){
			c = 0;
		}
		char *newarray = (char*) malloc(size * sizeof(char));
		if (newarray == NULL){
			printf(TXT_RED "Memory allocation failed\n" DEFAULT);
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

/* betolt egy FEN file-t adott fajlnevvel, beallitja a rablat es a metaadatokat*/
int loadFEN(char *filename, char board[12][12], bool *tomove, int castling[], squarenums *enpass, int *fmv, int *movenum){ 
	char *FEN;
	FEN = readFEN(filename);
	if (FEN == NULL){
		return 1;
	}
	//printf("%s:\n%s\n", filename, FEN);
	if (setboardFEN(FEN, board, tomove, castling, enpass, fmv, movenum)){
		printf(TXT_RED "Unable to load FEN!\n" DEFAULT);
		free(FEN);
		return 1;
	}
	free(FEN);
	return 0;
}
