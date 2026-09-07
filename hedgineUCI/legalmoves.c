#include "headers/legalmoves.h"

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
#define WPAWN_MOVE_FORWARD 0x0000FFFFFFFFFF00LL
#define WPAWN_PROMOTE      0x00FF000000000000LL
#define WPAWN_HOME         0x000000000000FF00LL

#define BPAWN_MOVE_FORWARD 0x00FFFFFFFFFF0000LL
#define BPAWN_PROMOTE      0x000000000000FF00LL
#define BPAWN_HOME         0x00FF000000000000LL

/* A knight standing at i can move to where knightMoves[i] is one 
 * */
const u64 knightMoves[64] = {132096LLU, 329728LLU, 659712LLU, 1319424LLU, 2638848LLU, 5277696LLU, 10489856LLU, 4202496LLU, 33816580LLU, 84410376LLU, 168886289LLU, 337772578LLU, 675545156LLU, 1351090312LLU, 2685403152LLU, 1075839008LLU, 8657044482LLU, 21609056261LLU, 43234889994LLU, 86469779988LLU, 172939559976LLU, 345879119952LLU, 687463207072LLU, 275414786112LLU, 2216203387392LLU, 5531918402816LLU, 11068131838464LLU, 22136263676928LLU, 44272527353856LLU, 88545054707712LLU, 175990581010432LLU, 70506185244672LLU, 567348067172352LLU, 1416171111120896LLU, 2833441750646784LLU, 5666883501293568LLU, 11333767002587136LLU, 22667534005174272LLU, 45053588738670592LLU, 18049583422636032LLU, 145241105196122112LLU, 362539804446949376LLU, 725361088165576704LLU, 1450722176331153408LLU, 2901444352662306816LLU, 5802888705324613632LLU, 11533718717099671552LLU, 4620693356194824192LLU, 288234782788157440LLU, 576469569871282176LLU, 1224997833292120064LLU, 2449995666584240128LLU, 4899991333168480256LLU, 9799982666336960512LLU, 1152939783987658752LLU, 2305878468463689728LLU, 1128098930098176LLU, 2257297371824128LLU, 4796069720358912LLU, 9592139440717824LLU, 19184278881435648LLU, 38368557762871296LLU, 4679521487814656LLU, 9077567998918656LLU};


/* A white pawn standing at place i can take to wpawnTakes[i].
 * 
 * A white king standing at place i is in check, if a black pawn is at 
 * wpawnTakes[i]. */
const u64 wpawnTakes[64] = {512LLU, 1280LLU, 2560LLU, 5120LLU, 10240LLU, 20480LLU, 40960LLU, 16384LLU, 131072LLU, 327680LLU, 655360LLU, 1310720LLU, 2621440LLU, 5242880LLU, 10485760LLU, 4194304LLU, 33554432LLU, 83886080LLU, 167772160LLU, 335544320LLU, 671088640LLU, 1342177280LLU, 2684354560LLU, 1073741824LLU, 8589934592LLU, 21474836480LLU, 42949672960LLU, 85899345920LLU, 171798691840LLU, 343597383680LLU, 687194767360LLU, 274877906944LLU, 2199023255552LLU, 5497558138880LLU, 10995116277760LLU, 21990232555520LLU, 43980465111040LLU, 87960930222080LLU, 175921860444160LLU, 70368744177664LLU, 562949953421312LLU, 1407374883553280LLU, 2814749767106560LLU, 5629499534213120LLU, 11258999068426240LLU, 22517998136852480LLU, 45035996273704960LLU, 18014398509481984LLU, 144115188075855872LLU, 360287970189639680LLU, 720575940379279360LLU, 1441151880758558720LLU, 2882303761517117440LLU, 5764607523034234880LLU, 11529215046068469760LLU, 4611686018427387904LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU};


/* A black pawn standing at place i can take to bpawnTakes[i].
 * 
 * A black king standing at place i is in check, if a white pawn is at 
 * bpawnTakes[i]. */
const u64 bpawnTakes[64] = {0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 0LLU, 2LLU, 5LLU, 10LLU, 20LLU, 40LLU, 80LLU, 160LLU, 64LLU, 512LLU, 1280LLU, 2560LLU, 5120LLU, 10240LLU, 20480LLU, 40960LLU, 16384LLU, 131072LLU, 327680LLU, 655360LLU, 1310720LLU, 2621440LLU, 5242880LLU, 10485760LLU, 4194304LLU, 33554432LLU, 83886080LLU, 167772160LLU, 335544320LLU, 671088640LLU, 1342177280LLU, 2684354560LLU, 1073741824LLU, 8589934592LLU, 21474836480LLU, 42949672960LLU, 85899345920LLU, 171798691840LLU, 343597383680LLU, 687194767360LLU, 274877906944LLU, 2199023255552LLU, 5497558138880LLU, 10995116277760LLU, 21990232555520LLU, 43980465111040LLU, 87960930222080LLU, 175921860444160LLU, 70368744177664LLU, 562949953421312LLU, 1407374883553280LLU, 2814749767106560LLU, 5629499534213120LLU, 11258999068426240LLU, 22517998136852480LLU, 45035996273704960LLU, 18014398509481984LLU};


/* A king standing at i attacks squares, where kingAttacks[i] is 1.
 * */
const u64 kingAttacks[64] = {770LLU, 1797LLU, 3594LLU, 7188LLU, 14376LLU, 28752LLU, 57504LLU, 49216LLU, 197123LLU, 460039LLU, 920078LLU, 1840156LLU, 3680312LLU, 7360624LLU, 14721248LLU, 12599488LLU, 50463488LLU, 117769984LLU, 235539968LLU, 471079936LLU, 942159872LLU, 1884319744LLU, 3768639488LLU, 3225468928LLU, 12918652928LLU, 30149115904LLU, 60298231808LLU, 120596463616LLU, 241192927232LLU, 482385854464LLU, 964771708928LLU, 825720045568LLU, 3307175149568LLU, 7718173671424LLU, 15436347342848LLU, 30872694685696LLU, 61745389371392LLU, 123490778742784LLU, 246981557485568LLU, 211384331665408LLU, 846636838289408LLU, 1975852459884544LLU, 3951704919769088LLU, 7903409839538176LLU, 15806819679076352LLU, 31613639358152704LLU, 63227278716305408LLU, 54114388906344448LLU, 216739030602088448LLU, 505818229730443264LLU, 1011636459460886528LLU, 2023272918921773056LLU, 4046545837843546112LLU, 8093091675687092224LLU, 16186183351374184448LLU, 13853283560024178688LLU, 144959613005987840LLU, 362258295026614272LLU, 724516590053228544LLU, 1449033180106457088LLU, 2898066360212914176LLU, 5796132720425828352LLU, 11592265440851656704LLU, 4665729213955833856LLU};

/* pops the least significant bit and returns an u64 with that bit only */
static inline u64 pop_lsb(u64 *var) {
	u64 lsb = *var & -*var;  // Isolate LS1B
	*var &= (*var - 1);      // Clear LS1B
	return lsb;
}

bool bitInCheck(const bitboard* const board, bool tomove){
	//~ printBitBoard2d(stdout, *board);
	
	int kingIndex = __builtin_ctzll((tomove == white ? board->piece[wking] : board->piece[bking]));
	
	if (tomove == white){
		if (board->piece[bpawn] & wpawnTakes[kingIndex]) return true;
		if (board->piece[bknight] & knightMoves[kingIndex]) return true;
	}
	else{
		if (board->piece[wpawn] & bpawnTakes[kingIndex]) return true;
		if (board->piece[wknight] & knightMoves[kingIndex]) return true;
	}

	{
		u64 friendly = enemypieces(board, tomove);
		u64 enemy = enemypieces(board, !tomove);
		int coloffset = (tomove == white ? bking : wking);
		u64 occ = enemy | friendly;
		
		u64 rookDir = board->piece[wrook + coloffset] | board->piece[wqueen + coloffset];
		if (Rmagic(kingIndex, occ) & rookDir) return true;

		u64 bishopDir = board->piece[wbishop + coloffset] | board->piece[wqueen + coloffset];
		if (Bmagic(kingIndex, occ) & bishopDir) return true;
		
		//~ printBitPieceAsBoard(possiblemoves);
	}

	//king
	if (board->piece[(tomove == white ? bking : wking)] & kingAttacks[kingIndex]){
		/* the king attacks are symmetrical, so it's enought to check one 
		 * side; if the orther attacks the first, the first attacks the other 
		 * too. */ 
		 return true;
	}
	
	return false;
}


static void addMoves(movearray* moves, int i, bool onlyCaptures, u64 enemy, u64 friendly, uint8_t piece, u64 possiblemoves){
	bitMove* legalmoves = moves->array;
	int* array_index = &(moves->size);
	
	if (onlyCaptures){
		u64 possiblecaptures = possiblemoves & enemy;
		
		while (possiblecaptures) { 
			u64 currentmove = pop_lsb(&possiblecaptures); 
			
			bitMove m = (bitMove){(size_t)i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			legalmoves[(*array_index)++] = m;			
		}
	}
	else {
		possiblemoves &= ~(enemy | friendly);
		
		while (possiblemoves) { 
			u64 currentmove = pop_lsb(&possiblemoves); 
			
			bitMove m = (bitMove){(size_t)i, __builtin_ctzll(currentmove), piece, -1, 0};
			legalmoves[(*array_index)++] = m;
		}
	}
}

static void addBitKnightMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = knightMoves[i];
	uint8_t piece = (tomove != white ? bknight : wknight);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}


static void addBitBishopMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = Bmagic(i, enemy | friendly); 
	uint8_t piece = (tomove != white ? bbishop : wbishop);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}
	
static void addBitRookMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = Rmagic(i, enemy | friendly); 
	uint8_t piece = (tomove != white ? brook : wrook);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}

static void addBitQueenMoves(movearray* moves, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	u64 possiblemoves = (Bmagic(i, enemy | friendly)) | (Rmagic(i, enemy | friendly)); 
	uint8_t piece = (tomove != white ? bqueen : wqueen);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
}

static void addBitKingMoves(movearray* moves, const bitboard* const board, bool tomove, int i, bool onlyCaptures, u64 enemy, u64 friendly){
	bitMove* legalmoves = moves->array;
	int* array_index = &(moves->size);
	
	u64 possiblemoves = kingAttacks[i];
	uint8_t piece = (tomove != white ? bking : wking);
	
	addMoves(moves, i, onlyCaptures, enemy, friendly, piece, possiblemoves);
	
	u64 allpieces = enemy | friendly;
	u64 mask = 1LLU << i;
	if (!onlyCaptures){
		if (tomove == white && i == 4) {
			if ((board->castlerights & WKINGSIDE) && ((allpieces & (mask << 1)) == 0) && ((allpieces & (mask << 2)) == 0)) {
				legalmoves[(*array_index)++] = (bitMove){i, 6, piece, -1, CASTLE_FLAG};
			}
			
			if ((board->castlerights & WQUEENSIDE) && ((allpieces & (mask >> 1)) == 0) && ((allpieces & (mask >> 2)) == 0) && ((allpieces & (mask >> 3)) == 0)) {
				legalmoves[(*array_index)++] = (bitMove){i, 2, piece, -1, CASTLE_FLAG};
			}
		}
		else if (tomove == black && i == 60) {
			if ((board->castlerights & BKINGSIDE) && ((allpieces & (mask << 1)) == 0) && ((allpieces & (mask << 2)) == 0)) {
				legalmoves[(*array_index)++] = (bitMove){i, 62, piece, -1, CASTLE_FLAG};
			}
			
			if ((board->castlerights & BQUEENSIDE) && ((allpieces & (mask >> 1)) == 0) && ((allpieces & (mask >> 2)) == 0) && ((allpieces & (mask >> 3)) == 0)) {
				legalmoves[(*array_index)++] = (bitMove){i, 58, piece, -1, CASTLE_FLAG};
			}
		}
	}
}

static void addBitPawnMoveWhite(movearray* moves, const bitboard* const board, u64 piecemask, int i, bool onlyCaptures, u64 enemy, u64 friendly){	
	int *array_index = &(moves->size);
	bitMove* legalmoves = moves->array;
		
	u64 moveforward = piecemask << 8;
	uint8_t piece = wpawn;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && (piecemask & WPAWN_MOVE_FORWARD) && !(moveforward & (enemy | friendly))){
		legalmoves[(*array_index)++] = (bitMove){i, i+8, piece, -1, 0};
		
		if ((piecemask & WPAWN_HOME) && 0 == ((piecemask << nort*2) & (enemy | friendly))){
			legalmoves[(*array_index)++] = (bitMove){i, i+16, piece, -1, DOUBLE_PAWNMOVE_FLAG};
		}
	}
	else if (!onlyCaptures){
		if ((piecemask & WPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			for (int prom = wqueen; prom <= wknight; prom++){
				legalmoves[(*array_index)++] = (bitMove){i, i+8, piece, prom, PROMOTION_FLAG};
			}
		}		
		return;
	}
	
		
	if (onlyCaptures){
		/* Takes / promotion */
		u64 possiblecaptures = wpawnTakes[i];
			
		while (possiblecaptures) { 	
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piecemask & WPAWN_MOVE_FORWARD)){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			}
			else if (board->enpassanttarget == currentmove){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG | EN_PASSANT_FLAG};
			}
			else if ((currentmove & enemy) && (piecemask & WPAWN_PROMOTE)) {				
				for (int prom = wqueen; prom <= wknight; prom++){
					legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, prom, CAPTURE_FLAG | PROMOTION_FLAG};
				}
			}
		}
	}
}

static void addBitPawnMoveBlack(movearray* moves, const bitboard* const board, u64 piecemask, size_t i, bool onlyCaptures, u64 enemy, u64 friendly){
	int *array_index = &(moves->size);
	bitMove* legalmoves = moves->array;
	
	u64 moveforward = piecemask >> 8;
	uint8_t piece = bpawn;
	
	/* check if we search for non-captures, and the pawn stands on a square 
	 * where it can move forward from. */ 
	if (!onlyCaptures && (piecemask & BPAWN_MOVE_FORWARD) && !(moveforward & (enemy | friendly))){
		legalmoves[(*array_index)++] = (bitMove){i, i-8, piece, -1, 0};
		
		if ((piecemask & BPAWN_HOME) && 0 == ((piecemask >> sout*2) & (enemy | friendly))){
			legalmoves[(*array_index)++] = (bitMove){i, i-16, piece, -1, DOUBLE_PAWNMOVE_FLAG};
		}
	}
	else if (!onlyCaptures){
		if ((piecemask & BPAWN_PROMOTE) && 0 == (moveforward & (enemy | friendly))){
			for (int prom = bqueen; prom <= bknight; prom++){
				legalmoves[(*array_index)++] = (bitMove){i, i-8, piece, prom, PROMOTION_FLAG};
			}
		}
		
		return;
	}
	
		
	if (onlyCaptures){
		/* Takes / promotion */
		u64 possiblecaptures = bpawnTakes[i];
			
		while (possiblecaptures) { 	
			u64 currentmove = pop_lsb(&possiblecaptures); 
			if ((currentmove & enemy) && (piecemask & BPAWN_MOVE_FORWARD)){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG};
			}
			else if (board->enpassanttarget == currentmove){
				legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, -1, CAPTURE_FLAG | EN_PASSANT_FLAG};
			}
			else if ((currentmove & enemy) && (piecemask & BPAWN_PROMOTE)) {				
				for (int prom = bqueen; prom <= bknight; prom++){
					legalmoves[(*array_index)++] = (bitMove){i, __builtin_ctzll(currentmove), piece, prom, CAPTURE_FLAG | PROMOTION_FLAG};
				}
			}
		}
	}
}

void bitGenerateLegalmoves(movearray* moves, const bitboard* const board, bool tomove, bool onlyCaptures){
	u64 enemy = enemypieces(board, !tomove); 
	u64 friendly = enemypieces(board, tomove);
	
	moves->size = 0; //delete anything that's there
	
	int coloffset = (tomove != white ? 6 : 0);
	
	movearray quiets;
	quiets.size = 0;
	
	u64 pieces = board->piece[wpawn + coloffset];
	if (tomove == black){
		while (pieces) {
			u64 mask = pop_lsb(&pieces);
			addBitPawnMoveBlack(moves, board, mask, __builtin_ctzll(mask), true, enemy, friendly);
			if (!onlyCaptures) addBitPawnMoveBlack(&quiets, board, mask, __builtin_ctzll(mask), false, enemy, friendly);
		}
	}
	else {
		while (pieces) {
			u64 mask = pop_lsb(&pieces);
			addBitPawnMoveWhite(moves, board, mask, __builtin_ctzll(mask), true, enemy, friendly);
			if (!onlyCaptures) addBitPawnMoveWhite(&quiets, board, mask, __builtin_ctzll(mask), false, enemy, friendly);
		}
	}
	pieces = board->piece[wqueen + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitQueenMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitQueenMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wbishop + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitBishopMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitBishopMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wknight + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitKnightMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitKnightMoves(&quiets, tomove,  __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wrook + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitRookMoves(moves, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitRookMoves(&quiets, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	pieces = board->piece[wking + coloffset];
	while (pieces) {
		u64 mask = pop_lsb(&pieces);
		addBitKingMoves(moves, board, tomove, __builtin_ctzll(mask), true, enemy, friendly);
		if (!onlyCaptures) addBitKingMoves(&quiets, board, tomove, __builtin_ctzll(mask), false, enemy, friendly);
	}
	
	// add quiet moves
	for (int i = 0; i < quiets.size; i++){
		moves->array[moves->size ++] = quiets.array[i]; 
	}
	
	//~ printf("--%d--\n--%d--\n", sizeof(bitboard), sizeof(movearray));
}

resultconst gameend(bitboard* board, bool tomove){
	movearray moves;
	
	bitGenerateLegalmoves(&moves, board, tomove, false);
	
	for (int i = 0; i < moves.size; i++){
		if (!isCastlingLegal(board, tomove, &moves.array[i])){
			continue;
		}
		bitUndo u = makeMove(board, moves.array[i]);
		bool check = bitInCheck(board, tomove);
		undoMove(board, moves.array[i], u);
		
		if (!check) return ONGOING; //we can make the move, doesn't result in check
	}
	
	if (bitInCheck(board, tomove)) return (tomove == white ? BLACKWON : WHITEWON);
	return DRAW;
}

/*returns true if:
 *   the move is not castling
 *    OR
 *   is castling and it is legal 
 * 
 * Note: the square we arrive on is not checked, as it would be redundant (that
 * is checked for every move anyway when said move is made) 
 * */
bool isCastlingLegal(bitboard* board, bool tomove, const bitMove* const mv){
	if ((mv->flags & CASTLE_FLAG)){
		if (bitInCheck(board, tomove)) { return false; }
		
		//check the pasing square if it's a check
		bitMove m = *mv;
		m.to = (m.from + m.to) / 2;
		m.flags = 0;
		
		bitUndo u = makeMove(board, m);
		bool con = bitInCheck(board, tomove);
		undoMove(board, m, u);
		
		if (con) { return false; }
	}
	return true;
}
