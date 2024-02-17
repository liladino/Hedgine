#include "legalmoves.h"

/*signum fv, annyi kulonbseggel, hogy a 0-ra is 1-et ad vissza*/
int signofn(int n){
	return n < 0 ? -1 : 1;
}

/*egesz szam abszolut erteket adja vissza*/
int abs(int a){
	return a >= 0 ? a : -a;
}

/*visszaallitja a kapott tablat alapallasba*/
void resetBoard(char board[12][12]){
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			if (i >= 2 && i < 10 && j >= 2 && j < 10)
				board[i][j] = ' ';
			else
				board[i][j] = 0;
		}
	}
	for (int i = 2; i < 6; i++){
		if (i == 2){
			board[2][i] = 'R'; board[2][11 - i] = 'R';
			board[9][i] = 'r'; board[9][11 - i] = 'r';
		}
		if (i == 3){
			board[2][i] = 'N'; board[2][11 - i] = 'N';
			board[9][i] = 'n'; board[9][11 - i] = 'n';
		}
		if (i == 4){
			board[2][i] = 'B'; board[2][11 - i] = 'B';
			board[9][i] = 'b'; board[9][11 - i] = 'b';
		}
		if (i == 5){
			board[2][i] = 'Q'; board[2][11 - i] = 'K';
			board[9][i] = 'q'; board[9][11 - i] = 'k';
		}
	}
	for (int i = 2; i < 10; i++){
		board[3][i] = 'P';
		board[8][i] = 'p';
	}
}

//kap egy tablat, es ha nincs maattado ero, egy erteket visszaad, egyebkent 0
int checkfordraw(char board[12][12]){
	int wking = 0, bking = 0, bishop = 0, knight = 0, other = 0;
	for(int i = 2; i < 10; i++){
		for(int j = 2; j < 10; j++){
			switch(board[i][j]){
				case 'K':
					wking++;
					break;
				case 'k':
					bking++;
					break;
				case 'b':
					bishop++;
					break;
				case 'B':
					bishop++;
					break;
				case 'n':
					knight++;
					break;
				case 'N':
					knight++;
					break;
				case ' ':
					break;
				default:
					other++;
			}
		}
	}
	if (wking != 1 || bking != 1){
		return 2;
	}
	if (other){
		return 0;
	}
	if (bishop < 2 && knight == 0){
		return 1;
	}
	if (bishop == 0 && knight < 2){
		return 1;
	}
	
	return 0;
}

/* egy tablan meglep egy kaptt lepest*/
void makemove(char board[12][12], move m){ //promotion: 0 = nincs, 'R', 'r', 'Q', 'q', ....
	squarenums from, to;
	char promotion = m.promotion;
	from = m.from;
	to = m.to;
	
	if (promotion == 0){
		if ( (board[from.rank][from.file] == 'K' || board[from.rank][from.file] == 'k') && abs(to.file - from.file) == 2){ 
			//sancolas
			if (to.file == 'c' - 'a' + 2){
				board[from.rank]['d' - 'a' + 2] = board[from.rank][2];
				board[from.rank][2] = ' '; //bastya helye
			}
			else{
				board[from.rank]['f' - 'a' + 2] = board[from.rank]['h' - 'a' + 2];
				board[from.rank]['h' - 'a' + 2] = ' ';
			}
		}
		if ((board[from.rank][from.file] == 'P' || board[from.rank][from.file] == 'p') && board[to.rank][to.file] == ' ' && abs(to.file-from.file) == 1){ 
			//en passant -> elmozdul a vonalrol => ut, de a mezo ahova erkezik ures
			board[from.rank][to.file] = ' '; //leutott gyalog
		}
		board[to.rank][to.file] = board[from.rank][from.file];
	}
	else{
		if (board[from.rank][from.file] == 'P')
			board[to.rank][to.file] = promotion;
		else
			board[to.rank][to.file] = promotion + 'a' -  'A';
	}
	board[from.rank][from.file] = ' ';
}

/* egy lepes metaadatait beallitja: sancolasi lehetoseg valtozasa, 
 * enpassant, 50 lepeses szabaly */
void setmeta(char board[12][12], move m, int castling[], squarenums *enpass, int *fmv){ //enpass, 50 lepeses szab, sanc beallitasa
	squarenums from, to;
	from = m.from;
	to = m.to;
	if (board[to.rank][to.file] != ' '){
		*fmv = 0;
	}
	else{
		(*fmv)++;
	}
	//ha a gyalog kettot ment, az en passant lehetoseg beallitasa
	int piece = board[from.rank][from.file];
	if (piece == 'P' || piece == 'p'){
		*fmv = 0;
		if  (abs(to.rank - from.rank) == 2){
			int enpasspiece;
			if (piece == 'P'){
				enpasspiece = 'p';
			}
			else{
				enpasspiece = 'P';
			}
			if ( board[to.rank][to.file - 1] == enpasspiece || board[to.rank][to.file + 1] == enpasspiece){ //csak akkor van en passant target square, ha van gyalog ami utne oda
				enpass->file = to.file;
				enpass->rank = (from.rank + to.rank)/2; 
			}
			else{
				enpass->file = -1;
				enpass->rank = -1;
			}
		}
		else{
			enpass->file = -1;
			enpass->rank = -1;
		}
	}
	else{
		enpass->file = -1;
		enpass->rank = -1;
	}
	
	//ha kiralylepes, sanc elvetele
	if (piece == 'K'){
		castling[0] = 0;
		castling[1] = 0;
	}
	if (piece == 'k'){
		castling[2] = 0;
		castling[3] = 0;
	}
	
	
	if ((from.rank == 2 && from.file == 9) || (to.rank == 2 && to.file == 9)){ //ha h1-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[0] = 0;
	}	
	if ((from.rank == 2 && from.file == 2) || (to.rank == 2 && to.file == 2)){ //ha a1-rol leptek, vagy oda leptek, vezeroldali sanc = 0
		castling[1] = 0;
	}
	if ((from.rank == 9 && from.file == 9) || (to.rank == 9 && to.file == 9)){ //ha h8-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[2] = 0;
	}
	if ((from.rank == 9 && from.file == 2) || (to.rank == 9 && to.file == 2)){ //ha h8-rol leptek, vagy oda leptek, kiralyoldali sanc = 0
		castling[3] = 0;
	}
	

}

/* kap egy tablat, es megmondja, sakkban all-e a lepesre allo fel.
 * ez vegzi el a kiralykozelites ellenorzeset is, ha sakknak vesszuk,
 * a mellekhatas ugyanaz, a kiraly nem lep sakkba, azaz masik kiraly melle*/
int incheck(char board[][12], bool tomove){ //0 ha nem sakk, 1 ha sakk, 2 ha hiba
	squarenums king = {-1, -1};
	char piece = 'K';
	char lookfor[7] = "RNBQPK";
	if (tomove){
		piece = 'a' + piece - 'A';	
	}
	else{
		for (int i = 0; i < 6; i++){
			lookfor[i] = 'a' + lookfor[i] - 'A';
		}
	}
	bool found = false;
	for (int i = 2; i < 10 && !found; i++){
		for (int j = 2; j < 10 && !found; j++){
			if (board[i][j] == piece){
				found = true;
				king.file = j;
				king.rank = i;
			}
		}
	}
	if (!found){
		return 2;
	}
	//futo iranyba nezni
	for (int x = -2; x <= 1; x++){ //i -> x kettes maradeka * 2 - 1 (-1, 1, -1, 1), j -> x elojele (-1, -1, 1, 1)
		int i = king.rank;
		int j = king.file;
		do {
			i += ((x+2) % 2) * 2 - 1;
			j += signofn(x);
		} while (board[i][j] == ' ');
		if (board[i][j] == lookfor[2] || board[i][j] == lookfor[3]){ //B vagy Q
			return 1;
		}
		
	}
	//gyalog:
	const int pawndir = (tomove ? -1 : 1);
	if ( board[king.rank + pawndir][king.file -1] == lookfor[4] || board[king.rank + pawndir][king.file + 1] == lookfor[4]){
		return 1;
	}
	//bastya iranyba nezni
	int i, j;
	for (int x = -2; x <= 1; x++){
		int directioni = signofn(x) * ((x+2) % 2), directionj = signofn(x) * ((x+3) % 2);
		i = king.rank + directioni; //-2 0;  -1 -1; 0 0; 1 1
		j = king.file + directionj; //-2 -1; -1  0; 0 1; 1 0
		while (board[i][j] == ' '){
			i += directioni;
			j += directionj;
		}
		if (board[i][j] == lookfor[0] || board[i][j] == lookfor[3]){
			return 1;
		}
	}
	
	//lolepes
	for (i = -1; i <= 1; i += 2){
		for (j = -1; j <= 1; j += 2){
			if (board[king.rank + 1 * j][king.file + 2 * i] == lookfor[1]){ //L alak bejaras elso resze
				return 1;
			}
			if (board[king.rank + 2 * j][king.file + 1 * i] == lookfor[1]){ //L alak bejaras masodik resze
				return 1;
			}
		}
	}
	
	//kiralykozelites
	for (i = -1; i <= 1; i++){
		for (j = -1; j <= 1; j++){
			if (j == 0 && i == 0){
				continue;
			}
			if (board[king.rank + j][king.file + i] == lookfor[5]){ 
				return 1;
			}
		}
	}
	
	return 0;
}

/* kap ket tablat, es a masodikat beallitja ugyanarra mint az elsot*/
void copyBoard(char from[12][12], char to[12][12]){
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 12; j++){
			to[i][j] = from[i][j];
		}
	}
}

/* kap egy lepest, es ha utana nem all sakkban a fel, hozzaadja a szabalyos 
 * lepesek listajahoz*/
void addmove(char board[12][12], char newboard[12][12], movelist *legalmoves, bool tomove, move m){
	copyBoard(board, newboard);
	makemove(newboard, m);
	if (incheck(newboard, tomove) == 0){
		insertMove(legalmoves, m);
		//printf("%c%d %c%d %c\n", m.from.file - 2 + 'a', m.from.rank-1, m.to.file - 2 + 'a', m.to.rank-1, m.promotion);
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos gyaloglepeseket*/
void addpawnmoves(char board[12][12], movelist *legalmoves, bool tomove, squarenums enpass, int i, int j){
	char newboard[12][12];
	const int direction = (!tomove ? 1 : -1);
	move m;
	m.from.file = j;
	m.from.rank = i;
	char prom[5] = {'R','N', 'B', 'Q'};
	if (board[i + direction][j] == ' '){
		m.to.file = j;
		m.to.rank = i + direction;
		if (i + direction == 2 || i + direction == 9){
			for (int y = 0; y < 4; y++){
				m.promotion = prom[y];
				addmove(board, newboard, legalmoves, tomove, m);
			}
			
		}
		else{
			m.promotion = 0;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
	if ((i == 3 && !tomove) || (i == 8 && tomove)){
		if (board[i + 2 * direction][j] == ' ' && board[i + direction][j] == ' '){
			m.to.file = j;
			m.to.rank = i + 2 * direction;
			m.promotion = 0;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
	//utes:
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	for (int y = -1; y <= 1; y += 2){
		m.to.file = j + y;
		m.to.rank = i + direction;
		if (board[i + direction][j + y] == 'R' + piececonvert || board[i + direction][j + y] == 'P' + piececonvert || board[i + direction][j + y] == 'Q' + piececonvert || board[i + direction][j + y] == 'B' + piececonvert || board[i + direction][j + y] == 'N' + piececonvert){
			
			if (i + direction == 2 || i + direction == 9){
				for (int z = 0; z < 4; z++){
					m.promotion = prom[z];
					addmove(board, newboard, legalmoves, tomove, m);
				}
			}
			else{
				m.promotion = 0;
				addmove(board, newboard, legalmoves, tomove, m);
			}
		}
		if (((i == 6 && !tomove) || (i == 5 && tomove)) && enpass.file == j + y && enpass.rank == i + direction){
			m.promotion = 0;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos huszarlepeseket*/
void addknightmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j){
	char newboard[12][12];
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	move m;
	m.from.file = j;
	m.from.rank = i;
	m.promotion = 0;
	for (int z = -1; z <= 1; z += 2){
		for (int y = -1; y <= 1; y += 2){
			if ((board[i + 1 * z][j + 2 * y] >= 'B' + piececonvert && board[i + 1 * z][j + 2 * y] <= 'R' + piececonvert) || board[i + 1 * z][j + 2 * y] == ' '){ //L alak bejaras elso resze
				m.to.file = j + 2 * y;
				m.to.rank = i + 1 * z;
				addmove(board, newboard, legalmoves, tomove, m);
			}
			if ((board[i + 2 * z][j + 1 * y] >= 'B' + piececonvert && board[i + 2 * z][j + 1 * y] <= 'R' + piececonvert) || board[i + 2 * z][j + 1 * y] == ' '){ //L alak bejaras masodik resze
				m.to.file = j + 1 * y;
				m.to.rank = i + 2 * z;
				addmove(board, newboard, legalmoves, tomove, m);
			}
			
		}
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos futolepeseket*/
void addbishopmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j){
	char newboard[12][12];
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	move m;
	m.from.file = j;
	m.from.rank = i;
	m.promotion = 0;
	for (int y = -2; y <= 1; y++){ //a -> y kettes maradeka * 2 - 1 (-1, 1, -1, 1), b -> y elojele (-1, -1, 1, 1)
		int directioni = ((y+2) % 2) * 2 - 1, directionj = signofn(y);
		int a = i, b = j;
		a += directioni;
		b += directionj;
		while (board[a][b] == ' '){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
			
			a += directioni;
			b += directionj;
		} 
		if ((board[a][b] >= 'B' + piececonvert && board[a][b] <= 'R' + piececonvert)){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos bastyalepeseket*/
void addrookmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j){
	char newboard[12][12];
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	move m;
	m.from.file = j;
	m.from.rank = i;
	m.promotion = 0;
	for (int y = -2; y <= 1; y++){
		int directioni = signofn(y) * ((y+2) % 2), directionj = signofn(y) * ((y+3) % 2);
		int a = i + directioni; //-2 0;  -1 -1; 0 0; 1 1
		int b = j + directionj; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
			
			a += directioni;
			b += directionj;
		}
		if ((board[a][b] >= 'B' + piececonvert && board[a][b] <= 'R' + piececonvert)){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos vezerlepeseket*/
void addqueenmoves(char board[12][12], movelist *legalmoves, bool tomove , int i, int j){
	char newboard[12][12];
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	move m;
	m.from.file = j;
	m.from.rank = i;
	m.promotion = 0;
	for (int y = -2; y <= 1; y++){ //futo irany
		int directioni = ((y+2) % 2) * 2 - 1, directionj = signofn(y);
		int a = i, b = j;
		a += directioni;
		b += directionj;
		while (board[a][b] == ' '){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
			
			a += directioni;
			b += directionj;
		} 
		if ((board[a][b] >= 'B' + piececonvert && board[a][b] <= 'R' + piececonvert)){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
	
	for (int y = -2; y <= 1; y++){ //bastya irany
		int directioni = signofn(y) * ((y+2) % 2), directionj = signofn(y) * ((y+3) % 2);
		int a = i + directioni; //-2 0;  -1 -1; 0 0; 1 1
		int b = j + directionj; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
			
			a += directioni;
			b += directionj;
		}
		if ((board[a][b] >= 'B' + piececonvert && board[a][b] <= 'R' + piececonvert)){
			m.to.file = b;
			m.to.rank = a;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
}

/* a szabalyos lepesek listajaba felveszi a szabalyos kiralylepeseket*/
void addkingmoves(char board[12][12], movelist *legalmoves, bool tomove, int castling[] , int i, int j){
	char newboard[12][12];
	const int piececonvert = (!tomove ? ('a' - 'A') : 0);
	move m;
	m.from.file = j;
	m.from.rank = i;
	m.promotion = 0;
	for (int y = -1; y <= 1; y++){ 
		for (int z = -1; z <= 1; z++){ 
			if (y == 0 && z == 0){
				continue;
			}
			if (board[i + y][j + z] == ' ' || (board[i + y][j + z] >= 'B' + piececonvert && board[i + y][j + z] <= 'R' + piececonvert)){
				m.to.file = j + z;
				m.to.rank = i + y;
				addmove(board, newboard, legalmoves, tomove, m);
			}
		}
	}
	//sanc:
	if (incheck(board, tomove)){
		return; //sakkbol nem lehet sancolni
	}
	
	int castlerank = 2;
	if (tomove){
		castlerank = 9;
	}
	if (castling[0 + tomove * 2] == 1 && board[castlerank][7] == ' ' && board[castlerank][8] == ' '){
		m.to.file = 7;
		m.to.rank = castlerank;
		copyBoard(board, newboard);
		makemove(newboard, m);
		if (incheck(newboard, tomove) == 0){
			m.to.file = 8;
			m.to.rank = castlerank;
			addmove(board, newboard, legalmoves, tomove, m);
		}
	}
	if (castling[1 + tomove * 2] == 1 && board[castlerank][3] == ' ' && board[castlerank][4] == ' ' && board[castlerank][5] == ' '){
		m.to.file = 5;
		m.to.rank = castlerank;
		copyBoard(board, newboard);
		makemove(newboard, m);
		if (incheck(newboard, tomove) == 0){
			m.to.file = 4;
			m.to.rank = castlerank;
			addmove(board, newboard, legalmoves, tomove, m);
		}
		
	}
}

/* generalja az osszes szabalyos lepest egy adott allasban, felszabaditas a hivo dolga*/
int generatemoves(char board[12][12], movelist *legalmoves, bool tomove, int castling[4], squarenums enpass ){ // 0, ha sikeres, 1, ha nincs szabalyos lepes
	const int piececonvert = (tomove ? ('a' - 'A') : 0);
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board[i][j] - piececonvert){
				case 'P':
					addpawnmoves(board, legalmoves, tomove, enpass, i, j);
					break;
				case 'N':
					addknightmoves(board, legalmoves, tomove, i, j);
					break;
				case 'B':
					addbishopmoves(board, legalmoves, tomove, i, j);
					break;
				case 'R':
					addrookmoves(board, legalmoves, tomove, i, j);
					break;
				case 'Q':
					addqueenmoves(board, legalmoves, tomove, i, j);
					break;
				case 'K':
					addkingmoves(board, legalmoves, tomove, castling, i, j);
			}
		}
	}

	return 0;
}

// true ha nem talalt szabalzos lepest, false ha talalt, felszabaditas a hivo dolga
bool endofgame(char board[12][12], movelist *legalmoves, bool tomove, int castling[4], squarenums enpass ){ 
	const int piececonvert = (tomove ? ('a' - 'A') : 0);
	int knight = 0, bishop = 0, other = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board[i][j] - piececonvert){
				case 'P':
					addpawnmoves(board, legalmoves, tomove, enpass, i, j);
					other++;
					break;
				case 'N':
					addknightmoves(board, legalmoves, tomove, i, j);
					knight++;
					break;
				case 'B':
					addbishopmoves(board, legalmoves, tomove, i, j);
					bishop++;
					break;
				case 'R':
					addrookmoves(board, legalmoves, tomove, i, j);
					other++;
					break;
				case 'Q':
					addqueenmoves(board, legalmoves, tomove, i, j);
					other++;
					break;
				case 'K':
					addkingmoves(board, legalmoves, tomove, castling, i, j);
			}
			if (legalmoves->next != NULL){
				if (checkfordraw(board) == 1) return true;
				return false;
			}
		}
	}
	return true;
}

/* osszehasonlit ket tablat (csak a jatszmaban fontos reszeiket), 
 * es true hogyha ugyanolyanok, egyebkent false*/
bool compareboards(char board1[12][12], char board2[12][12]){
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			if (board1[i][j] != board2[i][j]){
				return false;
			}
		}
	}
	return true;
}

/* ket sanc tombot osszehasonlit, es ha ugyanaz, true*/
bool comparecastle(int castling1[4], int castling2[4]){
	for (int i = 0; i < 4; i++){
		if (castling1[i] != castling2[i]){
			return false;
		}
	}
	return true;
}


bool repetition(char startboard[12][12], movelist *start, bool s_tomove, int s_castling[4], squarenums s_enpass, char board[12][12], bool tomove, int castling[4], squarenums enpass){
	char currboard[12][12];
	copyBoard(startboard, currboard);
	movelist *current;
	current = start;
	if (current->m.from.file == -1){
		current = current->next;
	}
	int repeats = 1;
	while (current != NULL){
		if (tomove == s_tomove && enpass.rank == s_enpass.rank && enpass.file == s_enpass.file && comparecastle(s_castling, castling) && compareboards(currboard, board)){
			//printboard(currboard, 0, 0, 0);
			repeats++;
			if (repeats > 2){
				return true;
			}
		}
		int temp = 0;
		setmeta(currboard, current->m, s_castling, &s_enpass, &temp);
		makemove(currboard, current->m);
		current = current->next;
	}
	return false;
}



/*
int main(){
	char board[12][12] = {
		{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
		{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
		{0, 0, 'R', ' ', ' ', ' ', 'K', ' ', ' ', 'R', 0, 0}, 
		{0, 0, 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 0, 0},
		{0, 0, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0, 0},
		{0, 0, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0, 0},
		{0, 0, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0, 0},
		{0, 0, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0, 0},
		{0, 0, 'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p', 0, 0},
		{0, 0, 'r', ' ', ' ', ' ', 'k', ' ', ' ', 'r', 0, 0},
		{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
		{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}
	};
	printf("%d %d\n", incheck(board, 0), incheck(board, 1));
	squarenums enpass = {2, 7};
	int castling[4] = {1, 1, 1, 1}; //KQkq
	//a legalmoves lehet hogy erdemes inkabb lancolt listava alakitani, de nem tudom
	int x = 0;
	move legalmoves[432]; //egy vezer lephet max 27 helyre, es minden figurat vezerrel felulbecsulve: 16*27=432 lepes
	generatemoves(board, legalmoves, 0, castling, enpass, &x);
	return 0;
}


*/

