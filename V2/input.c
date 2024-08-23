#include "input.h"

/* leellenorzi, egy lepes eleme e a szabalyos lepeseknek*/
bool checkiflegal(move m, movelist *legalmoves){ //1 ha szabalyos a lepes, 0, ha szabalytalan
	movelist *current = legalmoves;
	while (current != NULL) {
		if (m.from.file == current->m.from.file && m.from.rank == current->m.from.rank && m.to.file == current->m.to.file && m.to.rank == current->m.to.rank && m.promotion == current->m.promotion){
			return true;
		}
	//	printf("%c\n", current->m.from.file+'a'-2);
		current = current->next;
	}

	return false;
}

/* beallitja a piecefound valtozot: megszamolja, hogy az adott helyrol 
 * mehet-e oda figura => innen derul ki, kell-e egyertelmusites*/
bool setpiecefound(int *piecefound, square from, square to, movelist *legalmoves ){
	move temp;
	setmove(&temp, from, to, 0);
	if (checkiflegal(temp, legalmoves)){
		(*piecefound)++;
		return true;
	}
	return false;
}

/* egy lepesstringet atalakit lepesse, ha a figura gyalog.*/
int SANconvertPawn(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to;
	from.rank = -1; //nem valos ertekek, amik tesztre jok
	from.file = 'z';
	char promotion = 0;
	
	int i = 1;
	if (SAN_move[1] == 'x'){
		if (strlen(SAN_move) < 4) return 1; //a gyalogutes legalabb 4 char: exd4
		i++;
		if ('a' > SAN_move[i] || SAN_move[i] > 'h') return 1;
		
		from.file = SAN_move[0];
		if (!tomove){
			from.rank = SAN_move[i + 1] - '0' - 1; 
			if (board[from.rank + 1][from.file - 'a' + 2] != 'P') return 1;
		}
		else{
			from.rank = SAN_move[i + 1] - '0' + 1; 
			if (board[from.rank + 1][from.file - 'a' + 2] != 'p') return 1;
		}
		to.rank = SAN_move[i + 1] - '0';
		to.file = SAN_move[i];
		i++;
	}
	if ('1' > SAN_move[i] || SAN_move[i] > '8') return 1;
	
	if (from.rank == -1){
		to.file = SAN_move[0];
		to.rank = SAN_move[i] - '0';
		from.file = to.file;
		if (!tomove){
			if (board[(to.rank + 1) - 1][to.file - 'a' + 2] == 'P')
				from.rank = to.rank-1; 
			else{
				from.rank = to.rank-2; 
				if (board[from.rank + 1][from.file - 'a' + 2] != 'P') return 1;
			}
		}
		else{
			if (board[(to.rank + 1) + 1][to.file - 'a' + 2] == 'p')
				from.rank = to.rank+1; 
			else{
				from.rank = to.rank+2; 
				if (board[from.rank + 1][from.file - 'a' + 2] != 'p') return 1;
			}
		}
	}
		
	//atvaltozas
	if (SAN_move[i] == '8' || SAN_move[i] == '1'){
		i++;
		if (SAN_move[i] != '=') return 1;
		i++;
		if (SAN_move[i] == 'Q' || SAN_move[i] == 'R' || SAN_move[i] == 'N' || SAN_move[i] == 'B'){
			promotion = SAN_move[i];
			
		}
		else return 1;
	}
	if (SAN_move[i+1] != 0 && SAN_move[i+1] != '+' && SAN_move[i+1] != '#'){
		return 1;
	}
	
	

	setmove(m, from, to, promotion);
	
	
	
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	
	return 0;
}

/* egy lepesstringet atalakit lepesse, ha a figura kiraly.*/
int SANconvertKing(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to;
	from.rank = -1; 
	from.file = 'z';
	char piece = SAN_move[0];
	if (tomove) piece = 'a' + piece - 'A';
	int x = 1;
	if (SAN_move[x] == 'x'){
		x++;
	}
	if ('a' > SAN_move[x] || 'h' < SAN_move[x]){
		return 1;
	}
	to.file = SAN_move[x];
	x++;
	if ('1' > SAN_move[x] || '8' < SAN_move[x]){
		return 1;
	}
	to.rank = SAN_move[x] - '0';
	x++;
	if ( SAN_move[x] != 0 && SAN_move[x] != '+' && SAN_move[x] != '#' ){
		return 1;
	}
	
	for (int i = to.rank + 2; i >= to.rank; i--){
		for (int j = to.file - 'a' + 1; j <= to.file - 'a' + 3; j++){
			if (board[i][j] == piece){
				from.rank = i - 1;
				from.file = j + 'a' - 2;
			}
		}
	}
	if (from.file == 'z'){
		return 1;
	}
	if (board[to.rank + 1][to.file - 'a' + 2] != ' ' && strstr(SAN_move, "x") == NULL){
		return 1;
	}
	
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/* megviszgalja, hogy egy sanc string 
 * hasonlit-e a felhasznalo altal adottra, ha igen, true*/
bool arethesame(char *SAN_move, char *castle){
	if (strlen(SAN_move) - 1 > strlen(castle)){
		return false;
	}
	int i;
	for (i = 0; castle[i] != 0 && SAN_move[i] != 0; i++){
		if (castle[i] != SAN_move[i]){
			return false;
		}
	}
	if ( SAN_move[i] != 0 && SAN_move[i] != '+' && SAN_move[i] != '#' ){
		return false;
	}
	return true;
}

/* beallitja a lepest, ha sanc volt a string*/
int SANconvertCastle(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){
	square from, to;
	to.file = -1;
	from.file = 'e';
	char castle1[] = "O-O"; //o betukkel hivatalos, de a 0 is gyakori. PGN-ben O kell szerepeljen
	char castle2[] = "0-0";
	char longcastle1[] = "O-O-O";
	char longcastle2[] = "0-0-0";
	if (!tomove){
		if (board[1 + 1]['e' - 'a' + 2] == 'K'){
			from.rank = 1;
		}
		else{
			return 1;
		}
	}
	else {
		if (board[8 + 1]['e' - 'a' + 2] == 'k'){
			from.rank = 8;
		}
		else{
			return 1;
		}
	}
	to.rank = from.rank;
	if (arethesame(SAN_move, castle1) || arethesame(SAN_move, castle2)){
		to.file = 'g';
	}
	else if (arethesame(SAN_move, longcastle1) || arethesame(SAN_move, longcastle2)){
		to.file = 'c';	
	}
	
	if (to.file == -1){
		return 1;
	}
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/*ha volt utes, beallitja az erkezesi mezot a string alapjan*/
int ifTakesSetTo(char *SAN_move, int x, square *to){
	if (SAN_move[x] == 'x'){
		x++;
		if ('a' <= SAN_move[x] && SAN_move[x] <= 'h'){
			to->file = SAN_move[x];
		}
		else{
			return 1;
		}
		x++;
		if ('1' <= SAN_move[x] && SAN_move[x] <= '8'){
			to->rank = SAN_move[x] - '0';
			x++;
			if ( SAN_move[x] != 0 && SAN_move[x] != '+' && SAN_move[x] != '#' ){
				return 1;
			}
		}
		else{
			return 1;
		}
	}
	else return 1;
	return 0;
}

/* beallitja az erkezesi es indulasi mezot, utobbit, ha volt egyertelmusitve*/
int setFromAndTo(char *SAN_move, square *to, square *from){
	char *takes = strstr(SAN_move, "x");
	if (takes == NULL){
		if ('1' <= SAN_move[1] && SAN_move[1] <= '8'){ //pl N1e2
			from->rank = SAN_move[1] - '0';
			if ('a' > SAN_move[2] || SAN_move[2] > 'h'){
				return 1;
			}
			to->file = SAN_move[2];
			
			if ('1' > SAN_move[3] || SAN_move[3] > '8'){ 
				return 1;
			}
			to->rank = SAN_move[3] - '0';
			
			if ( SAN_move[4] != 0 && SAN_move[4] != '+' && SAN_move[4] != '#' ){
				return 1;
			}
		}
		else if ('a' <= SAN_move[1] && SAN_move[1] <= 'h'){
			if ('a' <= SAN_move[2] && SAN_move[2] <= 'h'){
				from->file = SAN_move[1];
				to->file = SAN_move[2];
				if ('1' > SAN_move[3] || SAN_move[3] > '8'){ 
					return 1;
				}
				to->rank = SAN_move[3] - '0';
				
				if ( SAN_move[4] != 0 && SAN_move[4] != '+' && SAN_move[4] != '#' ){
					return 1;
				}
			}
			else if ('1' <= SAN_move[2] && SAN_move[2] <= '8'){
				if ('a' <= SAN_move[3] && SAN_move[3] <= 'h'){
					from->file = SAN_move[1];
					from->rank = SAN_move[2] - '0';
					
					if ('1' <= SAN_move[4] && SAN_move[4] <= '8'){
						to->file = SAN_move[3];
						to->rank = SAN_move[4] - '0';
					}
					else{
						return 1;
					}
				}
				else{
					to->file = SAN_move[1];
					to->rank = SAN_move[2] - '0';
					if ( SAN_move[3] != 0 && SAN_move[3] != '+' && SAN_move[3] != '#' ){
						return 1;
					}
				}
			}
			else{
				return 1;
			}
		}
		else{
			return 1;
		}
		return 0;
	}
	else{
		if ('a' <= SAN_move[1] && SAN_move[1] <= 'h'){
			from->file = SAN_move[1];
			if ('1' <= SAN_move[2] && SAN_move[2] <= '8'){ //Qa1xa4
				from->rank = SAN_move[2] - '0';
				if ('x' != SAN_move[3]){
					return 1;
				}
				if (ifTakesSetTo(SAN_move, 3, to)){
					return 1;
				}
				if ( SAN_move[6] != 0 && SAN_move[6] != '+' && SAN_move[6] != '#' ){
					return 1;
				}
			}
			else{ 
				if ('x' != SAN_move[2]){ //Nexb5
					return 1;
				}
				if (ifTakesSetTo(SAN_move, 2, to)){
					return 1;
				}
				if ( SAN_move[5] != 0 && SAN_move[5] != '+' && SAN_move[5] != '#' ){
					return 1;
				}
			}
		}
		else if ('x' == SAN_move[1]){
			if (ifTakesSetTo(SAN_move, 1, to)){
				return 1;
			}
		}
		else if ('1' <= SAN_move[1] && SAN_move[1] <= '8' && 'x' == SAN_move[2]){ //N6xe5
			from->rank = SAN_move[1] - '0';
			if (ifTakesSetTo(SAN_move, 2, to)){
				return 1;
			}
			if ( SAN_move[5] != 0 && SAN_move[5] != '+' && SAN_move[5] != '#' ){
				return 1;
			}
		}
		else {
			return 1;
		}
		
	}
	
	return 0;
}

//elleorzi, a figura jo helyrol jott-e, ha nincs egyertelmusites, vagy egy egyertelmusites van, pl Ng3, Neg3, N2g3
int checkpiece(int i, char j, square possiblefrom){ 
	if (possiblefrom.rank == -1 && possiblefrom.file == 'z'){
		return 1;
	}
	else {
		if (possiblefrom.rank == i){
			return 1;
		} 
		if (possiblefrom.file == j){
			return 1;
		}
	}
	return 0;
}

/* egy lepesstringet atalakit lepesse, ha a figura huszar.*/
int SANconvertKnight(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to, possiblefrom;
	possiblefrom.rank = -1; 
	possiblefrom.file = 'z';
	
	char piece = SAN_move[0];
	if (tomove) piece = 'a' + piece - 'A';

	if (setFromAndTo(SAN_move, &to, &possiblefrom)){
		return 1;
	}

	if (possiblefrom.rank != -1 && possiblefrom.file != 'z'){
		if (board[possiblefrom.rank + 1][possiblefrom.file - 'a' + 2] != piece){
			return 1;
		}
		setmove(m, possiblefrom, to, 0);
		if (!checkiflegal(*m, legalmoves)){
			return 3;
		}
		return 0;
	}
	
	int piecefound = 0;
	for (int i = -1; i <= 1; i += 2){
		for (int j = -1; j <= 1; j += 2){
			if (board[to.rank + 1 + 1 * j][to.file - 'a' + 2 + 2 * i] == piece){ //L alak bejaras elso resze
				if (checkpiece(to.rank + 1 * j, to.file + 2 * i, possiblefrom)){
					square tempfrom;
					tempfrom.file = to.file + 2 * i;
					tempfrom.rank = to.rank + 1 * j;
					if (setpiecefound(&piecefound, tempfrom, to, legalmoves )){
						from.file = to.file + 2 * i;
						from.rank = to.rank + 1 * j;;
					}
				}
			}
			if (board[to.rank + 1 + 2 * j][to.file - 'a' + 2 + 1 * i] == piece){ //L alak bejaras masodik resze
				if (checkpiece(to.rank + 2 * j, to.file + 1 * i, possiblefrom)){
					square tempfrom;
					tempfrom.file = to.file + 1 * i;
					tempfrom.rank = to.rank + 2 * j;
					if (setpiecefound(&piecefound, tempfrom, to, legalmoves )){
						from.file = to.file + 1 * i;
						from.rank = to.rank + 2 * j;
					}
				}
			}
		}
	}
	if (piecefound > 1){
		return 2;
	}
	if (piecefound < 1){
		return 1;
	}
	
	if (board[to.rank + 1][to.file - 'a' + 2] != ' ' && strstr(SAN_move, "x") == NULL){
		return 1;
	}
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/* egy lepesstringet atalakit lepesse, ha a figura bastya.*/
int SANconvertRook(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to, possiblefrom;
	possiblefrom.rank = -1; 
	possiblefrom.file = 'z';
	
	char piece = SAN_move[0];
	if (tomove) piece = 'a' + piece - 'A';

	if (setFromAndTo(SAN_move, &to, &possiblefrom)){
		return 1;
	}

	if (possiblefrom.rank != -1 && possiblefrom.file != 'z'){
		if (board[possiblefrom.rank + 1][possiblefrom.file - 'a' + 2] != piece){
			return 1;
		}
		setmove(m, possiblefrom, to, 0);
		if (!checkiflegal(*m, legalmoves)){
			return 3;
		}
		return 0;
	}
	
	int piecefound = 0;
	int i = to.rank + 1;
	int j = to.file - 'a' + 2;
	for (int y = -2; y <= 1; y++){ //bastya irany
		int directioni = signofn(y) * ((y+2) % 2), directionj = signofn(y) * ((y+3) % 2);
		int a = i + directioni; //-2 0;  -1 -1; 0 0; 1 1
		int b = j + directionj; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			a += directioni;
			b += directionj;
		}
		if (board[a][b] == piece && checkpiece(a - 1, b + 'a' - 2, possiblefrom)){
			square tempfrom;
			tempfrom.file = b + 'a' - 2;
			tempfrom.rank = a - 1;
			if (setpiecefound(&piecefound, tempfrom, to, legalmoves )){
				from.file = b + 'a' - 2;
				from.rank = a - 1;
			}
		}
	}
	
	
	if (piecefound > 1){
		return 2;
	}
	if (piecefound < 1){
		return 1;
	}
	
	if (board[to.rank + 1][to.file - 'a' + 2] != ' ' && strstr(SAN_move, "x") == NULL){
		return 1;
	}
	
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/* egy lepesstringet atalakit lepesse, ha a figura futo.*/
int SANconvertBishop(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to, possiblefrom;
	possiblefrom.rank = -1; 
	possiblefrom.file = 'z';
	
	char piece = SAN_move[0];
	if (tomove) piece = 'a' + piece - 'A';

	if (setFromAndTo(SAN_move, &to, &possiblefrom)){
		return 1;
	}

	if (possiblefrom.rank != -1 && possiblefrom.file != 'z'){
		if (board[possiblefrom.rank + 1][possiblefrom.file - 'a' + 2] != piece){
			return 1;
		}
		setmove(m, possiblefrom, to, 0);
		if (!checkiflegal(*m, legalmoves)){
			return 3;
		}
		return 0;
	}
	
	int piecefound = 0;
	for (int x = -2; x <= 1; x++){ //i -> x kettes maradeka * 2 - 1 (-1, 1, -1, 1), j -> x elojele (-1, -1, 1, 1)
		int i = to.rank;
		char j = to.file;
		do {
			i += ((x+2) % 2) * 2 - 1;
			j += signofn(x);
		} while (board[i + 1][j - 'a' + 2] == ' ');
		if (board[i + 1][j - 'a' + 2] == piece && checkpiece(i, j, possiblefrom)){
			square tempfrom;
			tempfrom.file = j;
			tempfrom.rank = i;
			if (setpiecefound(&piecefound, tempfrom, to, legalmoves )){
				from.file = j;
				from.rank = i;
			}
		}
	}
	
	if (piecefound > 1){
		return 2;
	}
	if (piecefound < 1){
		return 1;
	}
	
	
	if (board[to.rank + 1][to.file - 'a' + 2] != ' ' && strstr(SAN_move, "x") == NULL){
		return 1;
	}
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/* egy lepesstringet atalakit lepesse, ha a figura vezer.*/
int SANconvertQueen(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves ){ 
	square from, to, possiblefrom;
	possiblefrom.rank = -1; 
	possiblefrom.file = 'z';
	
	char piece = SAN_move[0];
	if (tomove) piece = 'a' + piece - 'A';

	if (setFromAndTo(SAN_move, &to, &possiblefrom)){
		return 1;
	}

	if (possiblefrom.rank != -1 && possiblefrom.file != 'z'){
		if (board[possiblefrom.rank + 1][possiblefrom.file - 'a' + 2] != piece){
			return 1;
		}
		setmove(m, possiblefrom, to, 0);
		if (!checkiflegal(*m, legalmoves)){
			return 3;
		}
		return 0;
	}
	//futo algoritmusa
	int piecefound = 0;
	for (int x = -2; x <= 1; x++){ //i -> x kettes maradeka * 2 - 1 (-1, 1, -1, 1), j -> x elojele (-1, -1, 1, 1)
		int i = to.rank;
		char j = to.file;
		do {
			i += ((x+2) % 2) * 2 - 1;
			j += signofn(x);
		} while (board[i + 1][j - 'a' + 2] == ' ');
		if (board[i + 1][j - 'a' + 2] == piece && checkpiece(i, j, possiblefrom)){
			square tempfrom;
			tempfrom.file = j;
			tempfrom.rank = i;
			if (setpiecefound(&piecefound, tempfrom, to, legalmoves)){
				from.file = j;
				from.rank = i;
			}
		}
	}
	
	int i = to.rank + 1;
	int j = to.file - 'a' + 2;
	for (int y = -2; y <= 1; y++){ //bastya irany
		int directioni = signofn(y) * ((y+2) % 2), directionj = signofn(y) * ((y+3) % 2);
		int a = i + directioni; //-2 0;  -1 -1; 0 0; 1 1
		int b = j + directionj; //-2 -1; -1  0; 0 1; 1 0
		while (board[a][b] == ' '){
			a += directioni;
			b += directionj;
		}
		if (board[a][b] == piece && checkpiece(a - 1, b + 'a' - 2, possiblefrom)){
			square tempfrom;
			tempfrom.file = b + 'a' - 2;
			tempfrom.rank = a - 1;
			if (setpiecefound(&piecefound, tempfrom, to, legalmoves)){
				from.file = b + 'a' - 2;
				from.rank = a - 1;
			}
		}
	}
	
	
	if (piecefound > 1){
		return 2;
	}
	if (piecefound < 1){
		return 1;
	}
	
	if (board[to.rank + 1][to.file - 'a' + 2] != ' ' && strstr(SAN_move, "x") == NULL){
		return 1;
	}
	
	setmove(m, from, to, 0);
	if (!checkiflegal(*m, legalmoves)){
		return 3;
	}
	return 0;
}

/* ha jott egy string, vegigmegy, hogy melyik figuraval leptunk, 
 * es beallitja a lepest ez es az allas alapja*/
int gothroughpieces(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves){
	if (strlen(SAN_move) < 2) return 1;
	if ('a' <= SAN_move[0] && SAN_move[0] <= 'h'){
		return SANconvertPawn(SAN_move, m, board, tomove, legalmoves );
	}
	else{
		if (strlen(SAN_move) < 3) return 1;
		if (strstr(SAN_move, "0-0") != NULL || strstr(SAN_move, "O-O") != NULL){
			return SANconvertCastle(SAN_move, m, board, tomove, legalmoves );
		}
		
		switch (SAN_move[0]){
			case 'K':
				return SANconvertKing(SAN_move, m, board, tomove, legalmoves);
			case 'N' :
				return SANconvertKnight(SAN_move, m, board, tomove, legalmoves);
			case 'R' :
				return SANconvertRook(SAN_move, m, board, tomove, legalmoves);
			case 'B' :
				return SANconvertBishop(SAN_move, m, board, tomove, legalmoves);
			case 'Q' : 
				return SANconvertQueen(SAN_move, m, board, tomove, legalmoves);
			default: 
				return 1; 
			
		}
	}
	
	return 0;
}


/* kap egy stringet es egy allast, majd beallitja a lepest*/
int fromSANconvert(char *SAN_move, move *m, char board[12][12], bool tomove, movelist *legalmoves){
	int convertreturn = gothroughpieces(SAN_move, m, board, tomove, legalmoves);
	//volt utes jelzes, de nem utottunk semmit:
	if (strstr(SAN_move, "x") != NULL && board[m->to.rank][m->to.file] == ' ' && board[m->from.rank][m->from.file] != 'P' && board[m->from.rank][m->from.file] != 'p'){
		return 1;
	}
	return convertreturn;
	
}
