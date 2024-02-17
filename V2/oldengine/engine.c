#include "engine.h"

/* visszaadja egy lebegopontos szam absolut erteket*/
double fabs(double a){
	return (a < 0 ? -a : a);
}

double openingking[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0, 0.5,   1,   1,-0.5, 0.3,  -1,   1,   1, 0, 0}, 
	{0, 0,-0.5,-0.8,-0.8,-0.8,-0.8,  -1,-0.8,-0.5, 0, 0},
	{0, 0,  -1,-1.5,-1.5,  -2,  -2,-1.5,-1.5,  -1, 0, 0},
	{0, 0,  -2,  -2,  -2,  -3,  -3,  -2,  -2,  -2, 0, 0},
	{0, 0,  -2,  -3,  -4,  -4,  -4,  -4,  -3,  -2, 0, 0},
	{0, 0,  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -3, 0, 0},
	{0, 0,  -6,  -5,-4.5,-4.5,-4.5,-4.5,  -5,  -6, 0, 0},
	{0, 0,  -7,  -6,  -5,-4.5,-4.5,  -5,  -6,  -7, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double middleking[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0, 0.5, 0.5, 0.4,-0.3,-0.3, 0.3, 0.5, 0.5, 0, 0}, 
	{0, 0,-0.3,-0.3,-0.5,-0.8,-0.8,-0.5,-0.3,-0.3, 0, 0},
	{0, 0,  -1,-1.5,-1.5,  -2,  -2,-1.5,-1.5,  -1, 0, 0},
	{0, 0,  -2,  -2,  -2,  -3,  -3,  -2,  -2,  -2, 0, 0},
	{0, 0,  -2,  -3,  -4,  -4,  -4,  -4,  -3,  -2, 0, 0},
	{0, 0,  -4,  -4,  -4,  -4,  -4,  -4,  -4,  -3, 0, 0},
	{0, 0,  -6,  -5,-4.5,-4.5,-4.5,-4.5,  -5,  -6, 0, 0},
	{0, 0,  -7,  -6,  -5,-4.5,-4.5,  -5,  -6,  -7, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double openingknight[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,  -1,-0.1,  -1,  -1,  -1,  -1,-0.1,  -1, 0, 0}, //1
	{0, 0,   0,   0,   0,0.35,0.35,   0,   0,   0, 0, 0}, //2
	{0, 0, 0.1, 0.2, 0.4,   0,   0, 0.4, 0.2, 0.1, 0, 0}, //3
	{0, 0, 0.2,   0, 0.4, 0.6, 0.6, 0.4,   0, 0.2, 0, 0}, //4
	{0, 0, 0.3, 0.3,0.55,0.39,0.39,0.55, 0.3, 0.3, 0, 0}, //5
	{0, 0, 0.3, 0.5, 0.6, 0.6, 0.6, 0.6, 0.5, 0.3, 0, 0}, //6
	{0, 0, 0.5, 0.6,   1, 0.7, 0.7,   1, 0.6, 0.5, 0, 0}, //7
	{0, 0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double middleknight[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,  -1,-0.1,-0.4,   0,   0,-0.4,-0.1,  -1, 0, 0}, //1
	{0, 0,   0,   0, 0.1, 0.4, 0.4, 0.1,   0,   0, 0, 0}, //2
	{0, 0, 0.1, 0.2,0.45,   0,   0,0.45, 0.2, 0.1, 0, 0}, //3
	{0, 0, 0.2,   0, 0.5,0.55,0.55, 0.5,   0, 0.2, 0, 0}, //4
	{0, 0, 0.2, 0.3,0.55,0.55,0.55,0.55, 0.3, 0.2, 0, 0}, //5
	{0, 0, 0.3, 0.5, 0.6, 0.6, 0.6, 0.6, 0.5, 0.3, 0, 0}, //6
	{0, 0, 0.5, 0.6, 0.6, 0.7, 0.7, 0.6, 0.6, 0.5, 0, 0}, //7
	{0, 0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double openingbishop[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,   0,-0.2,-0.1,-0.2,-0.2,-0.1,-0.2,   0, 0, 0}, //1
	{0, 0, 0.2, 0.5,   0, 0.2, 0.2,   0, 0.5, 0.2, 0, 0}, //2
	{0, 0,-0.1, 0.2, 0.1,   0,   0, 0.1, 0.2,-0.1, 0, 0}, //3
	{0, 0, 0.3,-0.1,0.35, 0.2, 0.2,0.35,-0.1, 0.3, 0, 0}, //4
	{0, 0, 0.2,0.31,   0, 0.2, 0.2,   0,0.31, 0.2, 0, 0}, //5
	{0, 0, 0.3, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.3, 0, 0}, //6
	{0, 0, 0.2, 0.1, 0.2, 0.1, 0.1, 0.3, 0.1, 0.2, 0, 0}, //7
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double middlebishop[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,   0,-0.2,-0.1,-0.2,-0.2,-0.1,-0.2,   0, 0, 0}, //1
	{0, 0, 0.2, 0.5,   0, 0.2, 0.2,   0, 0.5, 0.2, 0, 0}, //2
	{0, 0,-0.1, 0.2, 0.1,   0,   0, 0.1, 0.2,-0.1, 0, 0}, //3
	{0, 0, 0.4,-0.1, 0.5, 0.2, 0.2, 0.5,-0.1, 0.4, 0, 0}, //4
	{0, 0, 0.2, 0.5,   0, 0.2, 0.2,   0, 0.5, 0.2, 0, 0}, //5
	{0, 0, 0.4, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.4, 0, 0}, //6
	{0, 0, 0.2, 0.1, 0.2, 0.1, 0.1, 0.3, 0.1, 0.2, 0, 0}, //7
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double openingpawn[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //1
	{0, 0, 0.0, 0.0,-0.1,-0.2,-0.2, 0.1, 0.0, 0.0, 0, 0}, //2
	{0, 0, 0.1, 0.4, 0.1, 0.4, 0.4,-0.1,   0, 0.1, 0, 0}, //3
	{0, 0,   0,-0.3, 0.5, 0.5, 0.4,-0.1,-0.3,   0, 0, 0}, //4
	{0, 0,   0,   0,   0,0.45,0.45,   0,   0,   0, 0, 0}, //5
	{0, 0,   1,   1,   1,   1,   1,   1,   1,   1, 0, 0}, //6
	{0, 0,   2,   2,   2,   2,   2,   2,   2,   2, 0, 0}, //7
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double middlepawn[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //1
	{0, 0, 0.0, 0.0,-0.1,-0.2,-0.2, 0.1, 0.0, 0.0, 0, 0}, //2
	{0, 0, 0.1, 0.1, 0.1, 0.2, 0.2,-0.2,   0, 0.1, 0, 0}, //3
	{0, 0,   0,-0.1, 0.4, 0.5, 0.5,-0.2,-0.1,   0, 0, 0}, //4
	{0, 0, 0.1, 0.1, 0.2, 0.5, 0.5, 0.1, 0.1, 0.1, 0, 0}, //5
	{0, 0,   1,   1,   1,   1,   1,   1,   1,   1, 0, 0}, //6
	{0, 0,   2,   2,   2,   2,   2,   2,   2,   2, 0, 0}, //7
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};


double middlerook[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,   0,   0, 0.4, 0.5, 0.5, 0.3,   0,   0, 0, 0}, //1
	{0, 0,   0,   0, 0.1, 0.2, 0.2, 0.1,   0,   0, 0, 0}, //2
	{0, 0,   0,   0, 0.1, 0.2, 0.2, 0.1,   0,   0, 0, 0}, //3
	{0, 0, 0.1, 0.1, 0.1, 0.3, 0.3, 0.1, 0.1, 0.1, 0, 0}, //4
	{0, 0, 0.1, 0.1, 0.1, 0.3, 0.3, 0.1, 0.1, 0.1, 0, 0}, //5
	{0, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0}, //6
	{0, 0,   1,   1,   1,   1,   1,   1,   1,   1, 0, 0}, //7
	{0, 0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};

double endgamecenter[12][12] = {
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, //feher old
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}, 
	{0, 0,-0.1,   0,   0,   0,   0,   0,   0,-0.1, 0, 0}, //1
	{0, 0,   0,0.03,0.04,0.05,0.05,0.04,0.03,   0, 0, 0}, //2
	{0, 0,   0,0.04,0.09, 0.1, 0.1,0.09,0.04,   0, 0, 0}, //3
	{0, 0,   0,0.05, 0.1,0.13,0.13, 0.1,0.05,   0, 0, 0}, //4
	{0, 0,   0,0.05, 0.1,0.13,0.13, 0.1,0.05,   0, 0, 0}, //5
	{0, 0,   0,0.04,0.09, 0.1, 0.1,0.09,0.04,   0, 0, 0}, //6
	{0, 0,   0,0.03,0.04,0.05,0.05,0.04,0.03,   0, 0, 0}, //7
	{0, 0,-0.1,   0,   0,   0,   0,   0,   0,-0.1, 0, 0}, //8
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0},
	{0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0, 0}  //sotet old
};




/* A megnyitasi fazishoz igazitott allasertekelo fv*/
double openingphase(char board[12][12]){
	double eval = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board[i][j]){ 
				case 'K':
					eval += openingking[i][j];
					break;
				case 'k':
					eval -= openingking[11-i][j];
					break;
				case 'Q':
					eval += 9;
					break;
				case 'q':
					eval -= 9;
					break;
				case 'R':
					eval += 4;
					break;
				case 'r':
					eval -= 4;
					break;
				case 'N':
					eval += 3 + openingknight[i][j];
					break;
				case 'n':
					eval -= 3 + openingknight[11-i][j];
					break;
				case 'B':
					eval += 3.1 + openingbishop[i][j];
					break;	
				case 'b':
					eval -= 3.1 + openingbishop[11-i][j];
					break;	
				case 'P':
					eval += 1 + openingpawn[i][j];
					break;
				case 'p':
					eval -= 1 + openingpawn[11-i][j];
					break;
			}
		}
	}
	return eval;
}

/* A kozepjatekhoz igazitott allasertekelo fv*/
double middlephase(char board[12][12]){
	double eval = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board[i][j]){ 
				case 'K':
					eval += middleking[i][j];
					break;
				case 'k':
					eval -= middleking[11-i][j];
					break;
				case 'Q':
					eval += 9 + (7 - piecedistance(board, 'k', 'Q')) * 0.2;
					break;
				case 'q':
					eval -= 9 + (7 - piecedistance(board, 'K', 'q')) * 0.2;
					break;
				case 'R':
					eval += 4.15 + middlerook[i][j];
					break;
				case 'r':
					eval -= 4.15 + middlerook[11-i][j];
					break;
				case 'N':
					eval += 3.1 + middleknight[i][j];
					break;
				case 'n':
					eval -= 3.1 + middleknight[11-i][j];
					break;
				case 'B':
					eval += 3.1 + middlebishop[i][j];
					break;	
				case 'b':
					eval -= 3.1 + middlebishop[11-i][j];
					break;	
				case 'P':
					eval += 1 + middlepawn[i][j];
					break;
				case 'p':
					eval -= 1 + middlepawn[11-i][j];
					break;
			}
		}
	}
	return eval;
}

int piecedistance(char board[12][12], char piece1, char piece2){
	int wj = 0, wi = 0, bj = 0, bi = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			if (board[i][j] == piece1){ 
				wj = j;
				wi = i;
			}
			else if (piece2 == board[i][j]){
				bi = i;
				bj = j;
			}
		}
	}
	return max(abs(wj - bj), abs(wi - bi));
}


/* A vegjatekhoz igazitott allasertekelo fv*/
double endgamephase(char board[12][12], int count){
	double eval = 0;
	double d = 7 - piecedistance(board, 'K', 'k'), wk = 0, bk = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			switch (board[i][j]){ 
				case 'K':
					wk = endgamecenter[i][j];
					break;
				case 'k':
					bk = endgamecenter[i][j];
					break;
				case 'Q':
					eval += 9 + (7 - piecedistance(board, 'k', 'Q')) * 0.1;
					break;
				case 'q':
					eval -= 9 + (7 - piecedistance(board, 'K', 'q')) * 0.1;
					break;
				case 'R':
					eval += 5;
					break;
				case 'r':
					eval -= 5;
					break;
				case 'N':
					eval += 3.15 + (7 - piecedistance(board, 'k', 'N')) * 0.2;
					break;
				case 'n':
					eval -= 3.15 + (7 - piecedistance(board, 'K', 'n')) * 0.2;
					break;
				case 'B':
					eval += 3.152 + endgamecenter[i][j];
					break;
				case 'b':
					eval -= 3.152 - endgamecenter[i][j];
					break;
				case 'P':
					eval += 1.3 + middlepawn[i][j];
					break;
				case 'p':
					eval -= 1.3 + middlepawn[11-i][j];
					break;
			}
		}
	}
	if (eval > 0){
		eval += d * 0.3;
		eval -= bk;
	}
	else{
		eval -= d * 0.3;
		eval += wk;
	}
	return eval;	
}


/* egy egyszeru ertekelo fv, ami a figurak mennyisege es helyzete 
 * alapjan ad egy valos szamot: 0 az egyenlo allas*/
double basic_eval(char board[12][12]){
	int count = countpieces(board);
	double eval = 0;
	if (count > 14){
		eval = openingphase(board);
	}
	else if (count > 6){
		eval = middlephase(board);
	}
	else {
		eval = endgamephase(board, count);
	}
	
	return eval;
}


/* megkapja a szabalyos lepesek listajat, es veletlenszeruen kivalaszt egyet belole*/
move lvl0(movelist *legalmoves){
	time_t t;
	srand((unsigned) time (&t));
	int i = 0;
	movelist *current = legalmoves;
	//printmoves(legalmoves);
	while (current != NULL){
		current = current->next;
		i++;
	}
	int n = rand() % (i-1) + 1;
	//printf("%d %d\n", i, n);
	current = legalmoves;
	i = 0;
	while (current != NULL){ 
		current = current->next;
		i++;
		if (i == n){
			return current->m;
		}
	}
	return current->m;
}

/* a teljes ertekelo fv, ami megnezi matt, patt van-e a tablan, vagy az 
 * egyszeru fv ertekelese mervado*/
double fulleval(char board[12][12], bool tomove, int castling[4], squarenums enpass){
	squarenums start = {-1, -1};
	movelist *legalmoves = createMove(initializemove(start, start, 0));
	
	if (endofgame(board, legalmoves, tomove, castling, enpass)){
		int checkcount = incheck(board, tomove);
		freemoves(legalmoves);
		if (checkcount != 0){
			return (!tomove ? blackwon : whitewon);
		}
		else{
			return stalemate;
		}
	}
	freemoves(legalmoves);
	int matingforce = checkfordraw(board);
	if (matingforce == 1){
		return draw;
	}
	/*if (checkcount != 0){ //a program rosszabbnak erzi az allast, ha sakkban all => pancser lat, sakkot beadja...
		return basic_eval(board) + (!tomove ? -0.05 : 0.05);
	}*/
	return basic_eval(board);
}

/* megszamolja a tablan a gyalogtol kulonbozo figurakat*/
int countpieces(char board[12][12]){
	int count = 0;
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			if ((board[i][j] >= 'a' &&  board[i][j] <= 'z' && board[i][j] != 'p') || (board[i][j] >= 'A' &&  board[i][j] <= 'Z' && board[i][j] != 'P'))
				count++;
		}
	}
	return count;
}

/* a teljes allast lemasolja uj valtozokba, tablaval metaval egyutt*/
void copymeta(char board[12][12], int castling[4], squarenums *enpass, char newboard[12][12], int newcastling[4], squarenums *newenpass){
	copyBoard(board, newboard);
	for (int i = 0; i < 4; i++){
		newcastling[i] = castling[i];
	}
	newenpass->file = enpass->file;
	newenpass->rank = enpass->rank;
}

//debug: kiirasa a legjobb lepes sorozatnak
//movewitheval line[11];

/* ket valos kozul a kisebbet adja vissza*/
double min(double a, double b){
	return (a < b ? a : b);
}
/* ket valos kozul a nagobbat adja vissza*/
double max(double a, double b){
	return (a > b ? a : b);
}

/* ket egesz kozul max*/
int intmax(int a, int b){
	return (a > b ? a : b);
}

/* a sakkkmotor, kap egy allast es egy melyseget, visszaad egy lepest ertekelessel*/
movewitheval CLCEngine(char board[12][12], bool tomove, int castling[4], squarenums enpass, int depth, int maxdepth, double alpha, double beta){
	movewitheval best = {{{-1, -1}, {-1, -1}, 0}, 0.0};
	
	if (depth >= maxdepth){
		best.eval = fulleval(board, tomove, castling, enpass);
		if (best.eval == stalemate){
			best.eval = 0;
		}
		return best;
	}
		
	double e = 0;
	const double currenteval = fulleval(board, tomove, castling, enpass);
	if (currenteval == stalemate){
		best.eval = 0;
		return best;
	}
	else if (currenteval == whitewon){
		best.eval = whitewon + maxdepth - depth;
		return best;
	}
	else if (currenteval == blackwon){
		best.eval = blackwon - maxdepth + depth;
		return best;
	}
	
	movelist *legalmoves = createMove(best.m);
	generatemoves(board, legalmoves, tomove, castling, enpass);
	movelist *current = legalmoves->next;
	
	best.m = legalmoves->m;
	best.eval = (!tomove ? INT_MIN : INT_MAX); 
	
	
	char newboard[12][12] = {0};
	int tempfmv = 0;
	int newcastling[4];
	squarenums newenpass;
	copymeta(board, castling, &enpass, newboard, newcastling, &newenpass);
	
	if (legalmoves->next->next == NULL){		
		best.m = legalmoves->next->m;
		best.eval = 0;
		if (depth > 0){
			setmeta(newboard, current->m, newcastling, &newenpass, &tempfmv);
			makemove(newboard, current->m);
			best.eval = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+1, maxdepth, alpha, beta).eval;
		}
		freemoves(legalmoves);
		return best;
	}
	
	while (current != NULL){
		if (board[current->m.to.rank][current->m.to.file] == ' ' && !(enpass.file == current->m.to.file && enpass.rank == current->m.to.rank)){
			current = current->next;
			continue;
		}
		copymeta(board, castling, &enpass, newboard, newcastling, &newenpass);
		setmeta(newboard, current->m, newcastling, &newenpass, &tempfmv);
		makemove(newboard, current->m);
		if (depth >= 3){
			if (incheck(newboard, 1-tomove))
				e = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+2, maxdepth, alpha, beta).eval;
			else
				e = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+3, maxdepth, alpha, beta).eval;
		}
		else {
			e = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+1, maxdepth, alpha, beta).eval;
		}
		if (!tomove){
			if (best.eval < e){ //vilagos
				best.m = current->m;
				best.eval = e;
			}
			if (best.eval >= beta + epsilon){
				break;
			}
			alpha = max(alpha, best.eval);
		}
		else { 
			if (best.eval > e){ //sotet
				best.m = current->m;
				best.eval = e;
			}
			if (best.eval <= alpha - epsilon){
				break;
			}
			beta = min(beta, best.eval);
		}
		
		current = current->next;
	}
	if (depth >= maxdepth / 2){
		if (depth >= maxdepth / 2 + 1){
			if (best.eval != INT_MAX && best.eval != INT_MIN){
				freemoves(legalmoves);
				return best;
			}	
		}
		else depth = maxdepth;
	}
	current = legalmoves->next;
	while (current != NULL){
		if (!(board[current->m.to.rank][current->m.to.file] == ' ' && !(enpass.file == current->m.to.file && enpass.rank == current->m.to.rank))){
			current = current->next;
			continue;
		}
		copymeta(board, castling, &enpass, newboard, newcastling, &newenpass);
		setmeta(newboard, current->m, newcastling, &newenpass, &tempfmv);
		makemove(newboard, current->m);
		//if (incheck(newboard, 1-tomove))
			//e = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+1, maxdepth, alpha, beta).eval;
		//else
			e = CLCEngine(newboard, 1-tomove, newcastling, newenpass, depth+1, maxdepth, alpha, beta).eval;
		
		if (!tomove){
			if (best.eval < e){ //vilagos
				best.m = current->m;
				best.eval = e;
			}
			if (best.eval >= beta + epsilon){
				break;
			}
			alpha = max(alpha, best.eval);
		}
		else { 
			if (best.eval > e){ //sotet
				best.m = current->m;
				best.eval = e;
			}
			if (best.eval <= alpha - epsilon){
				break;
			}
			beta = min(beta, best.eval);
		}
		
		current = current->next;
	}
	
	freemoves(legalmoves);
	return best;
}

bool foundpiecie(char board[12][12], char piece){
	for (int i = 2; i < 10; i++){
		for (int j = 2; j < 10; j++){
			if (board[i][j] == piece)
				return true;
		}
	}
	return false;
}

/* felhasznalo kivalasztja, hogy milyen erossegu sakkmotorral gondolkozzon a 
 * program, es ez visszaad a megfelelo erosseggel egy lepest*/
move CPU(int cpulvl, char board[12][12], bool tomove, int castling[4], squarenums enpass){
	squarenums start = {-1, -1};
	move temp = initializemove(start, start, 0);
	movelist *legalmoves = createMove(temp);
	generatemoves(board, legalmoves, tomove, castling, enpass);
	switch(cpulvl){
		case 0: //veletlenszeru
			usleep(millisec * 300);
			temp = lvl0(legalmoves);
			break;
		case 1:
			usleep(millisec * 300);
			temp = CLCEngine(board, tomove, castling, enpass, 0, 4, INT_MIN, INT_MAX).m;
			break;
		case 2:
			temp = CLCEngine(board, tomove, castling, enpass, 0, 6, INT_MIN, INT_MAX).m;
			break;
		case 3:
			if (countpieces(board) < 4 && !foundpiecie(board, 'Q') && !foundpiecie(board, 'q'))
				temp = CLCEngine(board, tomove, castling, enpass, 0, 10, INT_MIN, INT_MAX).m;
			else if (countpieces(board) < 5)
				temp = CLCEngine(board, tomove, castling, enpass, 0, 9, INT_MIN, INT_MAX).m;
			else 
				temp = CLCEngine(board, tomove, castling, enpass, 0, 8, INT_MIN, INT_MAX).m;
			break;
		default: 
			temp = lvl0(legalmoves);
	}
/*	printf("\n");
	for (int i = 0; line[i].m.from.file != 0; i++){
		printmove(line[i].m);
		printf(" %f\n", line[i].eval);
	}
	*/
	//printf("%d\n", piecedistance(board, 'K', 'k'));
	freemoves(legalmoves);
	return temp;
}
