#include "main.h"


int main(){
	printf("Long algebraic notation test\n----\nGive a move (e.g. e2e4):\n");
	char movestr[5] = {0};
	scanf("%5s", movestr);

	move m = readLongAlgebraicNotation(movestr);
	printmove(m);

	size_t limit = 10;
	printf("\n\nGetline test\n----\nGive strings (stops at %ld characters):\n", limit);
	char* str = NULL;
	int a;
	while((a = getLine(&str, limit))){
		printf("%s,\t%d\n", str, a);
	}
	printf("%s,\t%d\n", str, a);
	free(str);
	
	return 0;
}
