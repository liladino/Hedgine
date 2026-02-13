#include "headers/input.h"

static void growString(char** str, size_t capacity) {
	void *newstr = realloc(*str, capacity);
	if (newstr == NULL){
		fprintf(stderr, TXT_RED "Memory allocation failed\n" DEFAULT);
		exit(1);
	}
	*str = newstr;
}

/* Reads one physical line, skipping leading spaces/tabs only.
   - Ensures *str is NUL-terminated.
   - Returns length excluding NUL.
   - Returns 0 on empty line (just '\n') after skipping spaces/tabs.
   - Returns 0 on EOF with no characters read (and sets *str to empty string). */
int readLineDynamic(char **str, size_t maxSize) {
	if (!str || maxSize == 0) return 0;

	size_t size = 0;
	size_t cap = 32;

	growString(str, cap);
	(*str)[0] = '\0';

	int c = 0;
	
	while ((c = getc(stdin)) != EOF) {
		if (c == '\n') {
			(*str)[0] = '\0';
			return 0;
		}
		if (c != ' ' && c != '\t') {
			break;
		}
	}

	if (c == EOF) {
		(*str)[0] = '\0';
		return -1;
	}

	while (c != EOF && c != '\n' && size < maxSize) {
		if (size + 1 >= cap) {
			cap = cap * 3 / 2 + 8;
			growString(str, cap);
		}
		(*str)[size++] = (char)c;
		c = getc(stdin);
	}

	(*str)[size] = '\0';

	// consume the rest of the line
	if (size >= maxSize) {
		while (c != EOF && c != '\n') c = getc(stdin);
	}
	
	#ifdef DEBUG
	//echo input to log
	if (size > 0) fprintf(debugOutput, "--> %s\n", *str);
	#endif

	return size;
}


/* moves in the form of e.g. e1g1 or e7e8q
 * expects str to be at least 1 char long and ends with 0
 * */
move parseLongAlgebraicNotation(char str[]){
	//~ move NULLMOVE = {{-1, -1}, {-1, -1}, 0};
	move current = {{-1, -1}, {-1, -1}, 0};
	
	if (str[0] > 'h' || str[0] < 'a') return NULLMOVE;
	square from;
	from.file = str[0];
	
	if (str[1] > '8' || str[1] < '1') return NULLMOVE;
	from.rank = str[1] - '0';
	
	if (str[2] > 'h' || str[2] < 'a') return NULLMOVE;
	square to;
	to.file = str[2];
	
	if (str[3] > '8' || str[3] < '1') return NULLMOVE;
	to.rank = str[3] - '0';
	
	if (str[4] >= 'A' && str[4] <= 'Z') str[4] += 'a' - 'A'; 
	
	char prom = 0;
	if (str[4] >= 'a' && str[4] <= 'z'){
		if (str[4] != 'b' && str[4] != 'n' && str[4] != 'r' && str[4] != 'q') return NULLMOVE;
		prom = str[4];
	} 
	
	setmove(&current, from, to, prom);
	
	return current;
}

// FEN stuff

int setboardFEN(char FEN[], bitboard* bboard, bool *tomove, int *fmv, int *movenum){ 
	char board[12][12] = {0};
	int castling[4];
	squarenums enpass = {-1, -1};
	
	//start pos: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
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
					fprintf(stderr, "too wide rank!\n");
					return 1;
				}
				board[rank][file] = ' ';
			//	printboardLetters(board);
				file++;
			}
			file--;
		}
		else if (FEN[i] == 'r' || FEN[i] == 'R' || FEN[i] == 'n' || FEN[i] == 'N' || FEN[i] == 'b' || FEN[i] == 'B' || FEN[i] == 'q' || FEN[i] == 'Q' || FEN[i] == 'k' || FEN[i] == 'K' || FEN[i] == 'p' || FEN[i] == 'P' ){
			if (file > 9){
				fprintf(stderr, "too wide rank!\n");
				return 1;
			}
			board[rank][file] = FEN[i];
		//	printboardLetters(board);
		}
		else if (FEN[i] == ' '){
			boardread = false;
		}
		else{
			fprintf(stderr, "unknown symbol!\n");
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
		fprintf(stderr, "illegal number of kings on board!\n");
		return 1;
	}
	
	*bboard = boardConvert(board, castling, enpass, tomove);
	setMailBox(bboard);
	
	//i++;
	char* metadata = calloc( (strlen(FEN) + 1), sizeof(char));
	
	{
		int j = 0;
		int tokenlength = 0;
		while(FEN[i] != 0){
			metadata[j] = FEN[i];
			if (metadata[j] != ' '){
				if (tokenlength >= 5){ // longest str is KQkq, 4 char. 
					free(metadata);
					return 1;
				}
				tokenlength++;
			}
			else{
				tokenlength = 0;
			}
			i++; 
			j++;
		}
	}
	
	char tomove_char;
	char castle_str[5] = {0};
	char enpass_str[5] = {0};
	if (sscanf(metadata, "%c %4s %2s", &tomove_char, castle_str, enpass_str) != 3){
		free(metadata);	
		fprintf(stderr, "couldnt read metadata!\n");
		return 1;
	}
	
	if (tomove_char == 'w'){
		*tomove = white;
	}
	else if (tomove_char == 'b'){
		*tomove = black;
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
		enpass.file = -1;
		enpass.rank = -1;
	}
	else{
		if (enpass_str[0] >= 'a' && enpass_str[0] <= 'h'){
			enpass.file = enpass_str[0] - 'a' + 2;
		}
		else{
			free(metadata);	
			return 1;
		}
		if (enpass_str[1] == '3' || enpass_str[1] == '6'){
			enpass.rank = enpass_str[1] - '0' + 1;
		}
		else{
			free(metadata);	
			return 1;
		}
	}
	if (sscanf(metadata, "%*s %*s %*s %d %d", fmv, movenum) != 2){
		fprintf(stderr, "couldn't read move numbers\n");
		return 1;
		*fmv = 0;
		*movenum = 0; 
	}
	free(metadata);
	
	(*bboard) = boardConvert(board, castling, enpass, tomove);
	
	bboard->hashValue = hashPosition(bboard, *tomove);
	
	return 0;
}


void readFEN(char str[], bitboard* bboard, bool *tomove, int* fmv, int* movenum){
	//rnbqkbnr/ppp2ppp/4p3/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3
	char pos[64+7+1] = {0}; //overly cautius upper bound (64 squares + 7 slahshes + 0 at the end)
	char tomovestr[2] = {0};
	char castlerights[5] = {0};
	char enpassanttarget[3] = {0};
	sscanf(str, "%71s %1s %4s %2s %d %d", pos, tomovestr, castlerights, enpassanttarget, fmv, movenum);
	
	char FEN[129] = {0};
	char space[] = " ";
	strcat(FEN, pos);
	strcat(FEN, space);
	
	strcat(FEN, tomovestr);
	strcat(FEN, space);
	
	strcat(FEN, castlerights);
	strcat(FEN, space);
	
	strcat(FEN, enpassanttarget);
	strcat(FEN, space);
	
	char numbers[10] = {0};
	snprintf(numbers, sizeof(numbers), "%d %d", (int)((*fmv) % 100), (int)((*movenum) % 1000));
	strcat(FEN, numbers);
	
	if (setboardFEN(FEN, bboard, tomove, fmv, movenum)){
		setboardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", bboard, tomove, fmv, movenum);
	}
}
