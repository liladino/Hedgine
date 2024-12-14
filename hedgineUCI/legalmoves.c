#include "legalmoves.h"

#define WKINGSIDE 1
#define WQUEENSIDE 2
#define BKINGSIDE 4
#define BQUEENSIDE 8

/* PAWN MOVE MASKS
 * 
 * WHITE perspective
 * 
 * bitmask for 2 square moves:
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 11111111
 * 00000000 = 0x000000000000FF00LL
 * 
 * bitmask for 1 square move:
 * 00000000
 * 00000000
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 11111111
 * 00000000 = 0x0000FFFFFFFFFF00LL
 * 
 * WEST and EAST are mirrored in the bitboard, cos a1 is the lsb.
 * bitmask for taking west:
 * h8 00000000
 *    11111110
 *    11111110
 *    11111110
 *    11111110
 *    11111110
 *    11111110 a2
 *    00000000 = 0x00FEFEFEFEFEFE00LL
 * 
 * bitmask for taking east:
 * 00000000
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 01111111
 * 00000000 = 0x007F7F7F7F7F7F00LL
 * 
 * bitmask for promotion:
 * 00000000
 * 11111111
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000
 * 00000000 = 0x00FF000000000000LL
 * 
 * */
#define PAWN_TAKE_EAST     0x007F7F7F7F7F7F00LL
#define PAWN_TAKE_WEST     0x00FEFEFEFEFEFE00LL

#define WPAWN_MOVE_FORWARD 0x0000FFFFFFFFFF00LL
#define WPAWN_PROMOTE      0x00FF000000000000LL
#define WPAWN_HOME         0x000000000000FF00LL
#define WPAWN_ENP_TAKES    0x000000FF00000000LL

#define BPAWN_MOVE_FORWARD 0x00FFFFFFFFFF0000LL
#define BPAWN_PROMOTE      0x000000000000FF00LL
#define BPAWN_HOME         0x00FF000000000000LL

u64 whiteKingIndex;
u64 blackKingIndex;

static inline u64 enemypieces(bitboard board, bool enemy){
	if (enemy == black){ //black
		return (board.piece[bking] | board.piece[bqueen] | board.piece[bbishop] | board.piece[bknight] | board.piece[bpawn] | board.piece[brook]); 
	}
	return (board.piece[wking] | board.piece[wqueen] | board.piece[wbishop] | board.piece[wknight] | board.piece[wpawn] | board.piece[wrook]); 
}


bool bitInCheck(const bitboard* const board, bool tomove){
	/* A bitmap for each square where the king stands, where should the function
	 * look for enemy knights. */
	static u64 knightAttacks[64] = {132096LLU, 329728LLU, 659712LLU, 1319424LLU, 2638848LLU, 5277696LLU, 10489856LLU, 4202496LLU, 33816580LLU, 84410376LLU, 168886289LLU, 337772578LLU, 675545156LLU, 1351090312LLU, 2685403152LLU, 1075839008LLU, 8657044482LLU, 21609056261LLU, 43234889994LLU, 86469779988LLU, 172939559976LLU, 345879119952LLU, 687463207072LLU, 275414786112LLU, 2216203387392LLU, 5531918402816LLU, 11068131838464LLU, 22136263676928LLU, 44272527353856LLU, 88545054707712LLU, 175990581010432LLU, 70506185244672LLU, 567348067172352LLU, 1416171111120896LLU, 2833441750646784LLU, 5666883501293568LLU, 11333767002587136LLU, 22667534005174272LLU, 45053588738670592LLU, 18049583422636032LLU, 145241105196122112LLU, 362539804446949376LLU, 725361088165576704LLU, 1450722176331153408LLU, 2901444352662306816LLU, 5802888705324613632LLU, 11533718717099671552LLU, 4620693356194824192LLU, 288234782788157440LLU, 576469569871282176LLU, 1224997833292120064LLU, 2449995666584240128LLU, 4899991333168480256LLU, 9799982666336960512LLU, 1152939783987658752LLU, 2305878468463689728LLU, 1128098930098176LLU, 2257297371824128LLU, 4796069720358912LLU, 9592139440717824LLU, 19184278881435648LLU, 38368557762871296LLU, 4679521487814656LLU, 9077567998918656LLU};
	
	/* A bitmap for each square where the white king stands, where should the 
	 * function look for black pawns. */
	static u64 bpawnAttacks[64] = {512LLU, 1280LLU, 2560LLU, 5120LLU, 10240LLU, 20480LLU, 40960LLU, 16384LLU, 131072LLU, 327680LLU, 655360LLU, 1310720LLU, 2621440LLU, 5242880LLU, 10485760LLU, 4194304LLU, 33554432LLU, 83886080LLU, 167772160LLU, 335544320LLU, 671088640LLU, 1342177280LLU, 2684354560LLU, 1073741824LLU, 8589934592LLU, 21474836480LLU, 42949672960LLU, 85899345920LLU, 171798691840LLU, 343597383680LLU, 687194767360LLU, 274877906944LLU, 2199023255552LLU, 5497558138880LLU, 10995116277760LLU, 21990232555520LLU, 43980465111040LLU, 87960930222080LLU, 175921860444160LLU, 70368744177664LLU, 562949953421312LLU, 1407374883553280LLU, 2814749767106560LLU, 5629499534213120LLU, 11258999068426240LLU, 22517998136852480LLU, 45035996273704960LLU, 18014398509481984LLU, 144115188075855872LLU, 360287970189639680LLU, 720575940379279360LLU, 1441151880758558720LLU, 2882303761517117440LLU, 5764607523034234880LLU, 11529215046068469760LLU, 4611686018427387904LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU};
	
	/* A bitmap for each square where the black king stands, where should the 
	 * function look for white pawns. */
	static u64 wpawnAttacks[64] = {0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 2LLU, 5LLU, 10LLU, 20LLU, 40LLU, 80LLU, 160LLU, 64LLU, 512LLU, 1280LLU, 2560LLU, 5120LLU, 10240LLU, 20480LLU, 40960LLU, 16384LLU, 131072LLU, 327680LLU, 655360LLU, 1310720LLU, 2621440LLU, 5242880LLU, 10485760LLU, 4194304LLU, 33554432LLU, 83886080LLU, 167772160LLU, 335544320LLU, 671088640LLU, 1342177280LLU, 2684354560LLU, 1073741824LLU, 8589934592LLU, 21474836480LLU, 42949672960LLU, 85899345920LLU, 171798691840LLU, 343597383680LLU, 687194767360LLU, 274877906944LLU, 2199023255552LLU, 5497558138880LLU, 10995116277760LLU, 21990232555520LLU, 43980465111040LLU, 87960930222080LLU, 175921860444160LLU, 70368744177664LLU, 562949953421312LLU, 1407374883553280LLU, 2814749767106560LLU, 5629499534213120LLU, 11258999068426240LLU, 22517998136852480LLU, 45035996273704960LLU, 18014398509481984LLU};
	
	/* A bitmap for each square where the king stands, where should the 
	 * function look for the other king. */
	static u64 kingAttacks[64] = {770LLU, 1797LLU, 3594LLU, 7188LLU, 14376LLU, 28752LLU, 57504LLU, 49216LLU, 197123LLU, 460039LLU, 920078LLU, 1840156LLU, 3680312LLU, 7360624LLU, 14721248LLU, 12599488LLU, 50463488LLU, 117769984LLU, 235539968LLU, 471079936LLU, 942159872LLU, 1884319744LLU, 3768639488LLU, 3225468928LLU, 12918652928LLU, 30149115904LLU, 60298231808LLU, 120596463616LLU, 241192927232LLU, 482385854464LLU, 964771708928LLU, 825720045568LLU, 3307175149568LLU, 7718173671424LLU, 15436347342848LLU, 30872694685696LLU, 61745389371392LLU, 123490778742784LLU, 246981557485568LLU, 211384331665408LLU, 846636838289408LLU, 1975852459884544LLU, 3951704919769088LLU, 7903409839538176LLU, 15806819679076352LLU, 31613639358152704LLU, 63227278716305408LLU, 54114388906344448LLU, 216739030602088448LLU, 505818229730443264LLU, 1011636459460886528LLU, 2023272918921773056LLU, 4046545837843546112LLU, 8093091675687092224LLU, 16186183351374184448LLU, 13853283560024178688LLU, 144959613005987840LLU, 362258295026614272LLU, 724516590053228544LLU, 1449033180106457088LLU, 2898066360212914176LLU, 5796132720425828352LLU, 11592265440851656704LLU, 4665729213955833856LLU};
	
	int coloffset = tomove == white ? bking : wking;
	
	u64 piecemask = 1;
	if (tomove == white){
		piecemask = piecemask << whiteKingIndex;
	}
	else {
		piecemask = piecemask << blackKingIndex;
	}
	
	//pawn
	if (tomove == white){
		if (board->piece[bpawn] & bpawnAttacks[whiteKingIndex]) return true;
	}
	else{
		if (board->piece[wpawn] & wpawnAttacks[blackKingIndex]) return true;
	}
	
	//horsey
	if (tomove == white){
		if (board->piece[bknight] & knightAttacks[whiteKingIndex]) return true;
	}
	else{
		if (board->piece[wknight] & knightAttacks[blackKingIndex]) return true;
	}
	
	
	//~ u64 enemy = enemypieces(board, !tomove), friendly = enemypieces(board, tomove);
	
	//~ //rook & queen dirs
	//~ u64 searchmask = piecemask;
	//~ for (int i = file; i > 0; i--){
		//~ searchmask = searchmask >> west;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask;
	//~ for (int i = file; i < 7; i++){
		//~ searchmask = searchmask << east;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask;
	//~ for (int i = rank; i > 0; i--){
		//~ searchmask = searchmask >> sout;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask;
	//~ for (int i = rank; i < 7; i++){
		//~ searchmask = searchmask << nort;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wrook + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask;
	
	//~ //bishop & queen dirs
	//~ int i = rank, j = file;
	//~ while (i > 0 && j > 0){
		//~ searchmask = searchmask >> soWe; i--; j--;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask; i = rank, j = file;
	//~ while (i > 0 && j < 7){
		//~ searchmask = searchmask >> soEa;  i--; j++;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask; i = rank, j = file;
	//~ while (i < 7 && j < 7){
		//~ searchmask = searchmask << noEa; i++; j++;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	//~ searchmask = piecemask; i = rank, j = file;
	//~ while (i < 7 && j > 0){
		//~ searchmask = searchmask << noWe; i++; j--;
		//~ if (searchmask & friendly) break;
		//~ if ((searchmask & enemy)){
			//~ if ((searchmask & board.piece[wqueen + coloffset]) || (searchmask & board.piece[wbishop + coloffset])) return true;
			//~ break;
		//~ }
	//~ }
	
	//king
	if (board->piece[wking] & kingAttacks[blackKingIndex]){
		/* the king attacks are symmetrical, so it's enought to check one 
		 * side; if the orther attacks the first, the first attacks the other 
		 * too.*/ 
		 return true;
	}
	
	return false;
}

static inline void deletePiece(bitboard *board, const u64 movemask){
	//if a rook gets taken, remove the castlingrights for it
	if (board->piece[wrook] & movemask){
		if ((1LLU << 7) & movemask) board->castlerights &= ~WKINGSIDE;
		else {if (1LLU & movemask) board->castlerights &= ~WQUEENSIDE;}
	}
	else if (board->piece[brook] & movemask){
		if ((1LLU << (7 + nort * 7)) & movemask) board->castlerights &= ~BKINGSIDE;
		else {if ((1LLU << (nort * 7)) & movemask) board->castlerights &= ~BQUEENSIDE;}
	}
	
	u64 inverse = (~movemask);
	//dont need to delete the king, it can never get taken
	//~ board->piece[0] &= inverse; 
	board->piece[1] &= inverse;
	board->piece[2] &= inverse;
	board->piece[3] &= inverse;
	board->piece[4] &= inverse;
	board->piece[5] &= inverse;
	//~ board->piece[6] &= inverse;
	board->piece[7] &= inverse;
	board->piece[8] &= inverse;
	board->piece[9] &= inverse;
	board->piece[10] &= inverse;
	board->piece[11] &= inverse;
}

static inline void setcastlingrights(bitboard *board, bool tomove, u64 piece){
	/* check if we moved the rook from the corner, and if so, delete the 
	 * castling rights for that side. */
	if (tomove == white){
		if (piece & 1LL){
			//hashCastleO(board, 1);
			board->castlerights &= ~(WQUEENSIDE);
		}
		else if (piece & (1LLU << 7)){
			//hashCastleO(board, 0);
			board->castlerights &= ~(WKINGSIDE);
		}
	}
	else {
		if (piece & (1LLU << (7 * nort))){
			//hashCastleO(board, 3);
			board->castlerights &= ~(BQUEENSIDE);
		}
		if (piece & (1LLU << (7 + 7 * nort))){
			//hashCastleO(board, 2);
			board->castlerights &= ~(BKINGSIDE);
		}
	}
}


static void addBitKnightMoves(movearray* moves, bitboard board, bool tomove, u64 piece, bool onlyCaptures, u64 enemy, u64 friendly){	

}

static void addBitKingMoves(movearray* moves, bitboard board, bool tomove, u64 piece, bool onlyCaptures, u64 enemy, u64 friendly){

}

static void addBitPawnMoveWhite(movearray* moves, bitboard board, bool tomove, u64 piece, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitboard* legalmoves = moves->boards;
	
	bitboard copy = board;
	
	u64 withoutpiece = board.piece[wpawn] & (~piece);
	
	u64 moveforward = piece << 8, takewest = piece << noWe, takeeast = piece << noEa;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && piece & WPAWN_MOVE_FORWARD && !(moveforward & (enemy | friendly))){
		//moving forward 1 sqyare
		board.piece[wpawn] = withoutpiece | moveforward; 
		board.enpassanttarget = 0;
		if (!bitInCheck(&board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
			
			if (piece & WPAWN_HOME){
				//moving 2 squares
				board.piece[wpawn] = withoutpiece | (piece << 16); 
				board.enpassanttarget = moveforward;
				if (!bitInCheck(&board, tomove)) {
					legalmoves[*array_index] = board;
					(*array_index)++;
				}
			}
		}		
	}
	else if (!onlyCaptures){
		return;
	}
	if (piece & WPAWN_PROMOTE){
		/* Promotion is technically not taking, but it's good to consider it 
		 * first, among with other captures */		
		if (!(moveforward & (enemy | friendly))){
			board.enpassanttarget = 0;
			board.piece[wpawn] = withoutpiece;
			board.piece[wqueen] |= moveforward; 
			if (!bitInCheck(&board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			else{
				/* if we are in check after a promotion, that means we will be 
				 * in check for every oether promotion*/
				board = copy;
				goto no_forw_promo; 
			}
			
			board.piece[wqueen] ^= moveforward; 
			board.piece[wrook] |= moveforward; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wrook] ^= moveforward; 
			board.piece[wbishop] |= moveforward; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wbishop] ^= moveforward; 
			board.piece[wknight] |= moveforward; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board = copy;
		}
		
		
		no_forw_promo:
		if ((takewest & enemy) && (piece & PAWN_TAKE_WEST)){
			board.enpassanttarget = 0;
			board.piece[wpawn] = withoutpiece;
			board.piece[wqueen] |= takewest; 
			deletePiece(&board, takewest);
			
			if (!bitInCheck(&board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			else{
				/* if we are in check after a promotion, that means we will be 
				 * in check for every oether promotion*/
				board = copy;
				goto no_west_promo; 
			}
			
			board.piece[wqueen] ^= takewest; 
			board.piece[wrook] |= takewest; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wrook] ^= takewest; 
			board.piece[wbishop] |= takewest; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wbishop] ^= takewest; 
			board.piece[wknight] |= takewest; 
			legalmoves[*array_index] = board;  (*array_index)++;
			board = copy;
		}
		
		no_west_promo:
		if ((takeeast & enemy) && (piece & PAWN_TAKE_EAST)){
			board.enpassanttarget = 0;
			board.piece[wpawn] = withoutpiece;
			board.piece[wqueen] |= takeeast; 
			deletePiece(&board, takeeast);
			
			if (!bitInCheck(&board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			else{
				/* if we are in check after a promotion, that means we will be 
				 * in check for every oether promotion*/
				board = copy;
				goto take; 
			}
			
			board.piece[wqueen] ^= takeeast; 
			board.piece[wrook] |= takeeast; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wrook] ^= takeeast; 
			board.piece[wbishop] |= takeeast; 
			legalmoves[*array_index] = board;  (*array_index)++;
			
			board.piece[wbishop] ^= takeeast; 
			board.piece[wknight] |= takeeast; 
			legalmoves[*array_index] = board;  (*array_index)++;
			board = copy;
		}
	}
	
	take:
	if ((takewest & enemy) && (piece & PAWN_TAKE_WEST)){
		board.piece[wpawn] = withoutpiece | takewest;
		//delete the enemy piece
		deletePiece(&board, takewest);
		
		board.enpassanttarget = 0;
		
		if (!bitInCheck(&board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
		
		board = copy;
	}
	
	if ((takeeast & enemy) && (piece & PAWN_TAKE_EAST)){
		board.piece[wpawn] = withoutpiece | takeeast;
		//delete the enemy piece
		deletePiece(&board, takewest);
		
		board.enpassanttarget = 0;
		
		if (!bitInCheck(&board, tomove)) {
			legalmoves[*array_index] = board;
			(*array_index)++;
		}
		
		board = copy;
	}
	
	
	//en passant
	if (board.enpassanttarget && (piece & WPAWN_ENP_TAKES)){
		/* take west, if there's an enemy pawn there and the en passant target
		 * square is the square to take west */
		if ((board.piece[bpawn] & (piece >> west)) && (board.enpassanttarget & takewest)){
			board.piece[wpawn] = withoutpiece | takewest;
			//delete the enemy pawn
			board.piece[bpawn] &= ~(piece >> west);
			
			board.enpassanttarget = 0;
			
			if (!bitInCheck(&board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			
			board = copy;
		}
		else if ((board.piece[bpawn] & (piece >> east)) && (board.enpassanttarget & takeeast)){
			board.piece[wpawn] = withoutpiece | takeeast;
			//delete the enemy pawn
			board.piece[bpawn] &= ~(piece << east);
			
			board.enpassanttarget = 0;
			
			if (!bitInCheck(&board, tomove)) {
				legalmoves[*array_index] = board;
				(*array_index)++;
			}
			
			board = copy;
		}
	}
}


void bitGenerateLegalmoves(movearray* moves, bitboard board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	bitboard* legalmoves = moves->boards;
	moves->size = 0;
	
	/* find the kings and store them in the whiteKingMask and blackKingMask 
	 * global variables, for the check detection function to effectively use 
	 * it. */
	u64 wKingMask = 1, bKingMask = 1;
	for (int i = 0; i < 64; i++){
		if (board.piece[wking] & wKingMask){
			whiteKingIndex = i;
		}
		else if (board.piece[bking] & bKingMask){
			blackKingIndex = i;
		}
		wKingMask = wKingMask << 1;
		bKingMask = bKingMask << 1;
	}
	
	if (tomove == black){
		/* if we search the moves for black, start with the moves close to the
		 * white backrank.*/
		u64 mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask) {} 
			else if (board.piece[brook] & mask) {} 
			else if (board.piece[bbishop] & mask) {} 
			else if (board.piece[bknight] & mask) {}  
			else if (board.piece[bqueen] & mask) {}
			else if (board.piece[bking] & mask) {} 
			mask = mask << 1;
		}
		
		if (onlyCaptures) goto generate_hash;
		
		//look for captures
		mask = 1;

		for (int i = 0; i < 64; i++){
			if (board.piece[bpawn] & mask) {}
			else if (board.piece[brook] & mask) {} 
			else if (board.piece[bbishop] & mask) {} 
			else if (board.piece[bknight] & mask) {}  
			else if (board.piece[bqueen] & mask) {}
			else if (board.piece[bking] & mask) {} 
			mask = mask << 1;
		}
	}
	else {
		/* if we search the moves for white, start with the moves close to the
		 * black backrank.*/
		u64 mask = 1LLU << (7 + 7 * nort);

		for (int i = 0; i < 64; i++){
			if (board.piece[wpawn] & mask) {addBitPawnMoveWhite(moves, board, tomove, mask, true, enemy, friendly);} 
			else if (board.piece[wrook] & mask) {} 
			else if (board.piece[wbishop] & mask) {} 
			else if (board.piece[wknight] & mask) {}  
			else if (board.piece[wqueen] & mask) {}
			else if (board.piece[wking] & mask) {} 
			mask = mask >> 1;
		}
		
		if (onlyCaptures) goto generate_hash;
		
		//look for captures
		mask = 1LLU << (7 + 7 * nort);

		for (int i = 0; i < 64; i++){
			if (board.piece[wpawn] & mask) {addBitPawnMoveWhite(moves, board, tomove, mask, false, enemy, friendly);}
			else if (board.piece[wrook] & mask) {} 
			else if (board.piece[wbishop] & mask) {} 
			else if (board.piece[wknight] & mask) {}  
			else if (board.piece[wqueen] & mask) {}
			else if (board.piece[wking] & mask) {} 
			mask = mask >> 1;
		}
	}
	
	
	/* after these loops are done, we should iterate over the new legal 
	 * moves, and generate a hash for each */
	generate_hash:
	for (int i = 0; i < moves->size; i++){
		moves->boards[i].hashValue = hashPosition(&(moves->boards[i]), tomove);
	}
	
	
	// OLD SHIT:
	//~ addBitPawnMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitKnightMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitSlidingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	//~ addBitKingMoves(legalmoves, &moves->size, board, tomove, true, enemy, friendly);
	
	//~ if (onlyCaptures) return;
	
	//~ addBitSlidingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	//~ addBitKnightMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
	//~ addBitPawnMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);/**/
	//~ addBitKingMoves(legalmoves, &moves->size, board, tomove, false, enemy, friendly);
}

resultconst gameend(bitboard board, bool tomove){
	movearray moves;
	
	bitGenerateLegalmoves(&moves, board, tomove, true);
	if (moves.size != 0) return ongoing;
	bitGenerateLegalmoves(&moves, board, tomove, false);
	if (moves.size != 0) return ongoing;
	
	
	if (bitInCheck(&board, tomove)) return (tomove == white ? blackwon : whitewon);
	return draw;
}


/*
move converttomove(char oldboard[12][12], char newboard[12][12]){
	//from 2 boards generate a move
}
* 
* 
* */



