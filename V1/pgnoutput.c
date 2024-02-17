#include "pgnoutput.h"

/*kap egy tablat es egy lepest, es egy huszar lepest beallit, hogy kell-e 
 * egyertelmusiteni a vonalat vagy sort, amiket cim szerint megkap*/
void toSANconvertKnight(char board[12][12], move m, int *f, int *r){
	char piece = board[m.from.rank][m.from.file];
	char afterboard[12][12];
	copyBoard(board, afterboard);
	makemove(afterboard, m);
	
	bool fileamb = false, rankamb = 0;
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (afterboard[m.to.rank + j][m.to.file + 2 * i] == piece){ //L alak bejaras elso resze
				if (m.to.file + 2 * i == m.from.file)
					rankamb = true;
				else
					fileamb = true;
			}
			if (afterboard[m.to.rank + 2 * j][m.to.file + i] == piece){ //L alak bejaras masodik resze
				if (m.to.file + i == m.from.file)
					rankamb = true;
				else
					fileamb = true;
			}
		}
	}
	if (fileamb == false){
		*f = -1;
	}
	if (rankamb == false){
		*r = -1;
	}
}

/*kap egy tablat es egy lepest, es egy futo lepest beallit, hogy kell-e 
 * egyertelmusiteni a vonalat vagy sort, amiket cim szerint megkap*/
void toSANconvertBishop(char board[12][12], move m, int *f, int *r){
	char piece = board[m.from.rank][m.from.file];
	
	bool fileamb = false, rankamb = 0;
	for (int x = -2; x <= 1; x++){ //i -> x kettes maradeka * 2 - 1 (-1, 1, -1, 1), j -> x elojele (-1, -1, 1, 1)
		int a = m.to.rank;
		int b = m.to.file;
		do {
			a += ((x+2) % 2) * 2 - 1;
			b += signofn(x);
		} while (board[a][b] == ' ');
		if (board[a][b] == piece && !(a == m.from.rank && b == m.from.file)){
			if (b == m.from.file)
				rankamb = true;
			else
				fileamb = true;
		}
	}
	
	if (fileamb == false){
		*f = -1;
	}
	if (rankamb == false){
		*r = -1;
	}
}

/*kap egy tablat es egy lepest, es egy bastya lepest beallit, hogy kell-e 
 * egyertelmusiteni a vonalat vagy sort, amiket cim szerint megkap*/
void toSANconvertRook(char board[12][12], move m, int *f, int *r){
	char piece = board[m.from.rank][m.from.file];
	bool fileamb = false, rankamb = false;
	for (int i = -2; i <= 1; i++){ 
		int directiona = signofn(i) * ((i+2) % 2), directionb = signofn(i) * ((i+3) % 2);
		int a = m.to.rank + directiona; //-2 0;  -1 -1; 0 0; 1 1
		int b = m.to.file + directionb; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			a += directiona;
			b += directionb;
		}
		if (board[a][b] == piece && !(a == m.from.rank && b == m.from.file)){
			if (b == m.from.file)
				rankamb = true;
			else
				fileamb = true;
		}
	}
	
	if (fileamb == false){
		*f = -1;
	}
	if (rankamb == false){
		*r = -1;
	}
}

/*kap egy tablat es egy lepest, es egy vezer lepest beallit, hogy kell-e 
 * egyertelmusiteni a vonalat vagy sort, amiket cim szerint megkap*/
void toSANconvertQueen(char board[12][12], move m, int *f, int *r){
	char piece = board[m.from.rank][m.from.file];
	bool fileamb = false, rankamb = false;
	for (int i = -2; i <= 1; i++){ 
		int directiona = signofn(i) * ((i+2) % 2), directionb = signofn(i) * ((i+3) % 2);
		int a = m.to.rank + directiona; //-2 0;  -1 -1; 0 0; 1 1
		int b = m.to.file + directionb; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			a += directiona;
			b += directionb;
		}
		if (board[a][b] == piece && !(a == m.from.rank && b == m.from.file)){
			if (b == m.from.file)
				rankamb = true;
			else
				fileamb = true;
		}
	}
	for (int x = -2; x <= 1; x++){ //i -> x kettes maradeka * 2 - 1 (-1, 1, -1, 1), j -> x elojele (-1, -1, 1, 1)
		int a = m.to.rank;
		int b = m.to.file;
		do {
			a += ((x+2) % 2) * 2 - 1;
			b += signofn(x);
		} while (board[a][b] == ' ');
		if (board[a][b] == piece && !(a == m.from.rank && b == m.from.file)){
			if (b == m.from.file)
				rankamb = true;
			else
				fileamb = true;
		}
	}
	
	
	if (fileamb == false){
		*f = -1;
	}
	if (rankamb == false){
		*r = -1;
	}
}

/* kap egy tablat, egy lepest, egy stringet, es hogy a string
 * hanyadik karakteret kell irnia (ertek szerint, mert nohet)
 * beallitj az osszes figuranak, hogy kell-e egyertelmusiteni 
 * a vonalat vagy sort, ahonnan jott, majd ha kell beirja a lepes 
 * string megfelelo helyere az egyertelmusitest. */
void disambiguation(char board[12][12], move m, char *SAN, int *i){
	int rank = m.from.rank;
	int file = m.from.file;
	int piece = board[rank][file];
	//bool tomove = 0;
	if (piece > 'a' && piece < 'z'){
		//tomove = 1;
		piece += 'A' - 'a';
	}
	
	switch (piece){
		case 'K':
			file = -1;
			rank = -1;
			break;
		case 'N':
			toSANconvertKnight(board, m, &file, &rank);
			break;
		case 'R':
			toSANconvertRook(board, m, &file, &rank);
			break;
		case 'B':
			toSANconvertBishop(board, m, &file, &rank);
			break;
		case 'Q':
			toSANconvertQueen(board, m, &file, &rank);
			break;
	}
 	
	if (file != -1){
		SAN[*i] = file + 'a' - 2;
		(*i)++;
	}
	if (rank != -1){
		SAN[*i] = rank + '0' - 1;
		(*i)++;
	}
}

/* beirja a kapott fajlba a PGN metaadatokat, 
 * azaz az idot, a site es jatekos infokat
 * a playercol: 2-ket jateos, 0-vilagos az ember, 1-sotet az ember */
void printmeta(FILE *fptr, int playercol){
	time_t current_time;
    struct tm* time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    char white[6], black[6];
    if (playercol == 2){
		strcpy(white, "Human");
		strcpy(black, "Human");
	}
	else if (playercol == 1){
		strcpy(white, "CPU");
		strcpy(black, "Human");
	}
	else{
		strcpy(white, "Human");
		strcpy(black, "CPU");
	}
    fprintf(fptr, "[Site \"Command Line Chess\"]\n[Date \"%d.%02d.%02d\"]\n[White \"%s\"]\n[Black \"%s\"]\n", 1900 + time_info->tm_year, time_info->tm_mon + 1, time_info->tm_mday, white, black);
	
}

/* az altalanos fv, ami kap egy tablat, es rajta elvegzendo(! megtetel elotti !)
 * lepest, illetve egy eleg hosszu (boven max 10) stringet, amibe
 * a lepest be fogja irni. A sakkot/mattot nem jelzi. */
void convert_back(char board[12][12], move m, char *SAN){
	//a sakkot es mattot a hivo fv jelzi majd
	int i = 0;
	if (board[m.from.rank][m.from.file] != 'p' && board[m.from.rank][m.from.file] != 'P'){
		SAN[0] = board[m.from.rank][m.from.file];
		if (SAN[0] > 'a' && SAN[0] < 'z'){
			SAN[0] += 'A'-'a';
		}
		i++;
		disambiguation(board, m, SAN, &i);
	}
	
	if (board[m.to.rank][m.to.file] != ' '){
		if (board[m.from.rank][m.from.file] == 'p' || board[m.from.rank][m.from.file] == 'P'){
			SAN[0] = m.from.file + 'a' - 2;
			i++;
		}
		SAN[i] = 'x';
		i++;
	}
	
	
	SAN[i] = m.to.file + 'a' - 2;
	i++;
	SAN[i] = m.to.rank - 1 + '0';
	i++;
	
	if (SAN[0] < 'i' && SAN[0] >= 'a' && (m.to.rank == 9 || m.to.rank == 2)){
		SAN[i] = '=';
		i++;
		SAN[i] = m.promotion;
		i++;
	}
	SAN[i] = 0;
}


/* kap egy kezdoallast, egy lancolt listat a lepesekrol, 
 * a jatekos szinet, egy eredmenyt, es ebbol keszit egy PGN file-t, 
 * amibe beirja a SAN lepeseket. Beirja a sakk es matt jeleit is.*/
int savepgn(char *FEN, movelist *game, int playercol, int result){ //result: 0-meg tart, 1 feher mattot adott, 2 fekete mattot adott, 3 patt, 4 feher nyert, 5 fekete nyert, 6 dontetlen 
	FILE *fptr;
	char filename[50] = {0};
	setfilename(filename, ".pgn");

	
	if ((fptr = fopen(filename,"w")) == NULL){
		return 1;
	}
	
	printmeta(fptr, playercol);
	char board[12][12] = {0};
	bool tomove = 0;
	int movenum = 1;
	int castling[4] = {1, 1, 1, 1};
	squarenums enpass = {-1, -1};
	int fmv = 0;
	
	if (FEN == NULL){
		resetBoard(board);
	}
	else{
		
		FEN = readFEN(FEN);
		setboardFEN(FEN, board, &tomove, castling, &enpass, &fmv, &movenum);
		
		fprintf(fptr, "[FEN \"%s\"]\n", FEN);
		free(FEN);
	}
	char res[10] = {0};
	switch (result){
		case 0:
			res[0] = '*';
			break;
		case 1:
			strcpy(res, "1-0");
			break;
		case 2:
			strcpy(res, "0-1");
			break;
		case 3:
			strcpy(res, "1/2-1/2");
			break;
		case 4:
			strcpy(res, "1-0");
			break;
		case 5:
			strcpy(res, "0-1");
			break;
		case 6:
			strcpy(res, "1/2-1/2");
			break;
	}
	fprintf(fptr, "[Result \"%s\"]\n\n", res);
	
	
	movelist *current = game->next;
	while (current != NULL){
		if (!tomove){
			fprintf(fptr, "%d.", movenum);
			movenum++;
		}
		
		if ((board[current->m.from.rank][current->m.from.file] == 'K' || board[current->m.from.rank][current->m.from.file] == 'k') && abs(current->m.from.file - current->m.to.file) == 2){
			fprintf(fptr, "O-O");
			if (current->m.from.file - current->m.to.file == 2){
				fprintf(fptr, "-O");
			}
		}
		else{
			char *str = (char *)malloc(10 * sizeof(char));
			if (str == NULL){
				printf(TXT_RED "Memory allocation failed!\n" TXT_WHITE);
				exit(1);
			}
		
			convert_back(board, current->m, str);
			fprintf(fptr, "%s", str);
			free(str);
		}
		
		makemove(board, current->m);
		setmeta(board, current->m, castling, &enpass, &fmv);
		
		if (incheck(board, !tomove)){
			if (current->next != NULL){
				fprintf(fptr, "+");
			}
			else{
				if (result == 0 || result > 3){
					fprintf(fptr, "+");
				}
				else{
					if (result < 3){
						fprintf(fptr, "# ");
					}
					fprintf(fptr, "%s\n", res);
				}
			}
		}
		fprintf(fptr, " ");
		current = current->next;
		tomove = 1-tomove;
	}
	if (result == 0 || result > 2){
		fprintf(fptr, "%s\n", res);
	}
	fclose(fptr);
	printf("%s\t", filename);
	return 0;
}
