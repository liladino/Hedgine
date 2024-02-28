#include "moves.h"

/*atalakit egy mezot mezoszamokka
 */
squarenums makesquarenum(square a){
	squarenums b;
	b.file = a.file - 'a' + 2;
	b.rank = a.rank + 1;
	return b;
}

/* letrehoz egy lepest ket mezoszambol es egy atvaltozas ertekbol 
 * */
move initializemove(squarenums from, squarenums to, int promotion){
	move m;
	m.from.file = from.file;
	m.from.rank = from.rank;
	m.to.file = to.file;
	m.to.rank = to.rank;
	m.promotion = promotion;
	return m;
}

/* letrehoz egy lepest ket mezobol es egy atvaltozas ertekbol 
 * */
void setmove(move *m, square from, square to, int promotion){
	m->from = makesquarenum(from);
	m->to = makesquarenum(to);
	m->promotion = promotion;
}

/* movelist *head = createMove(m); 
 * letrehoz egy uj node-ot, folgal memoriat neki
 * */
movelist *createMove(move m) {
	movelist *newMove = (movelist*)malloc(sizeof(movelist));
	if (newMove == NULL) {
		printf(TXT_RED "Memory allocation failed\n"  TXT_WHITE);
		exit(1);
	}
	newMove->m = m;
	newMove->next = NULL;
	return newMove;
}

/* movelist *head; move m;
 * firstmove(&head, m); 
 * egy lepest a lista elejere fuz
 * */
void firstmove(movelist **head, move m) { //dupla pointer, mert megvaltoztatjuk a fv-en belül a *head erteket => **head-et kell atvenni
	movelist *newNode = createMove(m); 
	newNode->next = *head; 
	*head = newNode;
}

/* move m; movelist *head = createMove(m);
 * insertMove(head, m); 
 * egy lepest a lista vegere fuz
 * **mukodesehez kell kezdeti node!** */
void insertMove(movelist *head, move m) {
	movelist *newMove = createMove(m);
	movelist *i = head;
	movelist *tail = head;
	while (i != NULL) {
		i = i->next;
		if (i != NULL)
			tail = i;
	}
	tail->next = newMove;
}

/* movelist *head;
 * undoMove(&head); 
 * kitorli az utolso lepest
 * */
void undoMove(movelist **head) {
	if (*head == NULL || (*head)->next == NULL) { // ures lista vizsgalata, azert a kovetkezo is, mert van egy startnode, ami nem lepes meg
		return;
	}

	movelist *tail = *head;
	movelist *previous = NULL;
	
    while (tail->next != NULL) {
		previous = tail;
		tail = tail->next;
	}
	
	previous->next = NULL;	//az utolso elotti mutatojat beallitjuk NULL-ra
	free(tail);				//az utolsot felszabaditjuk
}

/* movelist *head;
 * printmoves(head); 
 * kiirja az osszes lepest a head elemtol
 * */
void printmoves(movelist *head) {
	movelist *current = head;
	current = current->next; //a startnode nem lepes
	while (current != NULL) {
		printmove(current->m);
		current = current->next;
	}
	printf("*\n");
}

/*kiir egy teljes lepest*/
void printmove(move m) {
	printf("%c%d%c%d%c", m.from.file+'a'-2, m.from.rank-1, m.to.file+'a'-2, m.to.rank-1, m.promotion-'A'+'a');
}

/* felszabaditj a lepeslistat
 * */
void freemoves(movelist *head) {
	while (head != NULL) {
		movelist *temp = head;
		head = head->next;
		free(temp);
	}
}

/* megszamolja a lepeseket
 * */
int countmoves(movelist *head) {
	movelist* a = head;
	int i = -1;
	while (head != NULL) {
		a = a->next;
		i++;
	}
	return i;
}


/*teszt
int main() {
	move m;
	m.from.file = 'e' - 'a' + 2;
	m.from.rank = 2 + 1;
	m.to.file = 'e' - 'a' + 2;
	m.to.rank = 4 + 1;
	movelist *head = createMove(m);
	
	//firstmove(&head, m);
	insertMove(head, m);
	undoMove(&head);
	//printf("Linked List: ");
	printmoves(head);

	freemoves(head);

	return 0;
}

*/
