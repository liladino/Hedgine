#include "fenoutput.h"

/* beallitja file nevet ahova mentunk*/
void setfilename(char *filename, char *extension){
	time_t current_time;
    struct tm* time_info;
    time(&current_time);
    time_info = localtime(&current_time);
	sprintf(filename, "%d-%02d-%02d-%02d-%02d-%02d%s", 1900 + time_info->tm_year, time_info->tm_mon + 1, time_info->tm_mday, time_info->tm_hour, time_info->tm_min, time_info->tm_sec, extension);
}

/* beirja az adott allast FEN-kent egy fajlba*/
int savefen(char board[12][12], bool tomove, int castling[], squarenums enpass, int fmv, int movenum){ 
	FILE *fptr;
	char filename[50] = {0};
	setfilename(filename, ".fen");
	if ((fptr = fopen(filename,"w")) == NULL){
		return 1;
	}
	int empty = 0; 
	for (int i = 9; i > 1; i--){
		for (int j = 2; j < 10; j++){
			char piece = board[i][j];
			if (piece != ' '){
				if (empty){
					fprintf(fptr, "%d", empty);
					empty = 0;
				}
				fprintf(fptr, "%c", piece);
			}
			else{
				empty++;
			}
		}
		if (empty){
			fprintf(fptr, "%d", empty);
		}
		empty = 0;
		if (i != 2){
			fprintf(fptr, "/");
		}
	}
	fprintf(fptr, " ");
	if (tomove){
		fprintf(fptr, "b");
	}
	else{
		fprintf(fptr, "w");
	}
	fprintf(fptr, " ");
	if (castling[0] + castling[1] + castling[2] + castling[3] == 0){
		fprintf(fptr, "-");
	}
	else{
		if (castling[0] == 1){
			fprintf(fptr, "K");
		}
		if (castling[1] == 1){
			fprintf(fptr, "Q");
		}
		if (castling[2] == 1){
			fprintf(fptr, "k");
		}
		if (castling[3] == 1){
			fprintf(fptr, "q");
		}
	}
	fprintf(fptr, " ");
	if (enpass.file == -1){
		fprintf(fptr, "-");
	}
	else{
		fprintf(fptr, "%c%d", enpass.file - 2 + 'a', enpass.rank - 1);
	}
	fprintf(fptr, " %d %d", fmv, movenum);
	printf("%s\t", filename);
	fclose(fptr);
	return 0;
}
