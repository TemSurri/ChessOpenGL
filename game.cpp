#include "game.h"
#include "vector"
#include <iostream>
#include <variant>


//PSUEDO LEGAL MOVE GENERATION
void ClassicChess::generate_pawn_moves(std::vector<Move>& moves, bool white)
{
	uint64_t pawns = white ? w_pawns : b_pawns;

	const PieceTypeBit pawnType = white ? W_PAWN : B_PAWN;
	const int forward = white ? NORTH : SOUTH;

	const int startMin = white ? 8 : 48;
	const int startMax = white ? 15 : 55;

	const uint64_t enemy = white ? b_occupancy : w_occupancy;

	while (pawns)
	{
		int from = pop_lsb(pawns);

		int oneForward = from + forward;

		// one-square push
		if (oneForward >= 0 && oneForward < 64 && is_set(empty, oneForward))
		{
			add_pawn_move(moves, from, oneForward, pawnType);

			// two-square push
			int twoForward = from + 2 * forward;

			if (from >= startMin && from <= startMax && is_set(empty, twoForward))
			{
				add_move(moves, from, twoForward, pawnType);
			}
		}

		// normal captures
		int capLeft = white ? from + NORTH_WEST : from + SOUTH_WEST;
		int capRight = white ? from + NORTH_EAST : from + SOUTH_EAST;

		if ((from % 8 != 0) && capLeft >= 0 && capLeft < 64 && is_set(enemy, capLeft))
		{
			add_pawn_move(moves, from, capLeft, pawnType);
		}

		if ((from % 8 != 7) && capRight >= 0 && capRight < 64 && is_set(enemy, capRight))
		{
			add_pawn_move(moves, from, capRight, pawnType);
		}

		// en passant
		if (en_passant_square != -1)
		{
			if ((from % 8 != 0) && capLeft == en_passant_square)
			{
				Move m;
				m.from = from;
				m.to = capLeft;
				m.moved = pawnType;
				m.captured = white ? B_PAWN : W_PAWN;
				m.type = EN_PASSANT_MOVE;

				moves.push_back(m);
			}

			if ((from % 8 != 7) && capRight == en_passant_square)
			{
				Move m;
				m.from = from;
				m.to = capRight;
				m.moved = pawnType;
				m.captured = white ? B_PAWN : W_PAWN;
				m.type = EN_PASSANT_MOVE;

				moves.push_back(m);
			}
		}
	}
}

void ClassicChess::generate_knight_moves(std::vector<Move>& moves, bool white)
{
	uint64_t knights = white ? w_knights : b_knights;
	uint64_t own = white ? w_occupancy : b_occupancy;

	PieceTypeBit knightType = white ? W_KNIGHT : B_KNIGHT;

	while (knights)
	{
		int from = pop_lsb(knights);

		uint64_t targets = knight_attacks[from] & ~own; //removes all the targets that overlap with your own occupancy

		while (targets)
		{
			int to = pop_lsb(targets);
			add_move(moves, from, to, knightType);
		}
	}
}

void ClassicChess::generate_king_moves(std::vector<Move>& moves, bool white)
{
	uint64_t king = white ? w_king : b_king;
	uint64_t own = white ? w_occupancy : b_occupancy;

	PieceTypeBit kingType = white ? W_KING : B_KING;

	if (!king) return;

	int from = pop_lsb(king);

	uint64_t targets = king_attacks[from] & ~own;

	while (targets)
	{
		int to = pop_lsb(targets);
		add_move(moves, from, to, kingType);
	}

	add_castling_moves(moves, white);
}

void ClassicChess::add_castling_moves(std::vector<Move>& moves, bool is_white)
{
	updateOccupancy();

	if (is_king_in_check(is_white))
		return;

	if (is_white)
	{
		if (white_can_castle_kingside &&
			(w_king & (1ULL << 4)) &&
			(w_rooks & (1ULL << 7)) &&
			!(occupancy & (1ULL << 5)) &&
			!(occupancy & (1ULL << 6)) &&
			!is_square_attacked(5, false) &&
			!is_square_attacked(6, false))
		{
			Move m;
			m.from = 4;
			m.to = 6;
			m.moved = W_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}

		if (white_can_castle_queenside &&
			(w_king & (1ULL << 4)) &&
			(w_rooks & (1ULL << 0)) &&
			!(occupancy & (1ULL << 1)) &&
			!(occupancy & (1ULL << 2)) &&
			!(occupancy & (1ULL << 3)) &&
			!is_square_attacked(3, false) &&
			!is_square_attacked(2, false))
		{
			Move m;
			m.from = 4;
			m.to = 2;
			m.moved = W_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}
	}
	else
	{
		if (black_can_castle_kingside &&
			(b_king & (1ULL << 60)) &&
			(b_rooks & (1ULL << 63)) &&
			!(occupancy & (1ULL << 61)) &&
			!(occupancy & (1ULL << 62)) &&
			!is_square_attacked(61, true) &&
			!is_square_attacked(62, true))
		{
			Move m;
			m.from = 60;
			m.to = 62;
			m.moved = B_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}

		if (black_can_castle_queenside &&
			(b_king & (1ULL << 60)) &&
			(b_rooks & (1ULL << 56)) &&
			!(occupancy & (1ULL << 57)) &&
			!(occupancy & (1ULL << 58)) &&
			!(occupancy & (1ULL << 59)) &&
			!is_square_attacked(59, true) &&
			!is_square_attacked(58, true))
		{
			Move m;
			m.from = 60;
			m.to = 58;
			m.moved = B_KING;
			m.type = CASTLING_MOVE;
			moves.push_back(m);
		}
	}
}




void ClassicChess::generate_bishop_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[4] = {
		NORTH_EAST,
		NORTH_WEST,
		SOUTH_EAST,
		SOUTH_WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_bishops : b_bishops,
		is_white,
		is_white ? W_BISHOP : B_BISHOP,
		dirs,
		4
	);
}

void ClassicChess::generate_rook_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[4] = {
		NORTH,
		SOUTH,
		EAST,
		WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_rooks : b_rooks,
		is_white,
		is_white ? W_ROOK : B_ROOK,
		dirs,
		4
	);
}

void ClassicChess::generate_queen_moves(std::vector<Move>& moves, bool is_white)
{
	static constexpr int dirs[8] = {
		NORTH,
		SOUTH,
		EAST,
		WEST,
		NORTH_EAST,
		NORTH_WEST,
		SOUTH_EAST,
		SOUTH_WEST
	};

	generate_sliding_moves(
		moves,
		is_white ? w_queen : b_queen,
		is_white,
		is_white ? W_QUEEN : B_QUEEN,
		dirs,
		8
	);
}



//LEGAL MOVE GENERATION
bool ClassicChess::is_square_attacked_by_sliders(int square, bool byWhite)
{
	static constexpr int bishopDirs[4] = {
		NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST
	};

	static constexpr int rookDirs[4] = {
		NORTH, SOUTH, EAST, WEST
	};

	for (int dir : bishopDirs)
	{
		int current = square;

		while (true)
		{
			int to = current + dir;

			if (!is_valid_slide(current, to, dir))
				break;

			uint64_t toMask = 1ULL << to;

			if (occupancy & toMask)
			{
				PieceTypeBit p = piece_on_square(to);

				if (byWhite && (p == W_BISHOP || p == W_QUEEN))
					return true;

				if (!byWhite && (p == B_BISHOP || p == B_QUEEN))
					return true;

				break;
			}

			current = to;
		}
	}

	for (int dir : rookDirs)
	{
		int current = square;

		while (true)
		{
			int to = current + dir;

			if (!is_valid_slide(current, to, dir))
				break;

			uint64_t toMask = 1ULL << to;

			if (occupancy & toMask)
			{
				PieceTypeBit p = piece_on_square(to);

				if (byWhite && (p == W_ROOK || p == W_QUEEN))
					return true;

				if (!byWhite && (p == B_ROOK || p == B_QUEEN))
					return true;

				break;
			}

			current = to;
		}
	}

	return false;
}

bool ClassicChess::is_square_attacked(int square, bool byWhite)
{
	updateOccupancy();

	uint64_t squareMask = 1ULL << square;

	// pawn attacks
	if (byWhite)
	{
		if (square % 8 != 0)
		{
			int pawnSquare = square + SOUTH_EAST; // square - 7
			if (pawnSquare >= 0 && pawnSquare < 64 && (w_pawns & (1ULL << pawnSquare)))
				return true;
		}

		if (square % 8 != 7)
		{
			int pawnSquare = square + SOUTH_WEST; // square - 9
			if (pawnSquare >= 0 && pawnSquare < 64 && (w_pawns & (1ULL << pawnSquare)))
				return true;
		}
	}
	else
	{
		if (square % 8 != 0)
		{
			int pawnSquare = square + NORTH_EAST; // square + 9
			if (pawnSquare >= 0 && pawnSquare < 64 && (b_pawns & (1ULL << pawnSquare)))
				return true;
		}

		if (square % 8 != 7)
		{
			int pawnSquare = square + NORTH_WEST; // square + 7
			if (pawnSquare >= 0 && pawnSquare < 64 && (b_pawns & (1ULL << pawnSquare)))
				return true;
		}
	}

	// knight attacks
	uint64_t attackingKnights = byWhite ? w_knights : b_knights;

	if (knight_attacks[square] & attackingKnights)
		return true;

	// king attacks
	uint64_t attackingKing = byWhite ? w_king : b_king;

	if (king_attacks[square] & attackingKing)
		return true;

	// bishop / rook / queen attacks
	if (is_square_attacked_by_sliders(square, byWhite))
		return true;

	return false;
}

















//DEBUG----------------------
void ClassicChess::printAllMoves() {

	std::cout << "Legal Moves" << std::endl;
	std::cout << white_move << std::endl;
	for (MoveSet moveSet : legalMoves) {

		

		for (auto end : moveSet.moves) {
			std::array<int, 2> start = { end.p->getRow(), end.p->getCol() };

			std::cout << "Piece(" << end.p->getType() << "):   " << '(' << start[0] << ", " << start[1] << ") -->" << '(' << end.r << ", " << end.c << ')' << end.fashion<< endl;

		}
	};

}

void ClassicChess::printBoard() {
	for (int r{}; r < BOARDROWS; r++) {
				for (int c{}; c < BOARDCOLS; c++) {

					if (board[r][c] == nullptr) {
						cout << 0 << " ";
					}
					else {
						cout << board[r][c]->getType() << " ";
					}
				};
				cout << endl;
			};
		return;
}
//SET UP-------------

Piece* ClassicChess::storePiece(int r, int c, PieceType type) {

	// assume row greater than 4 is white
	// this is where we can store differnt objects based on type but for not itll be on Piece class later can do inhertiance

	bool upper = r < BOARDROWS / 2;
	bool is_white = (upper == white_upper);

	Piece piece = Piece(r, c, is_white, type, board);

	if (is_white) {
		whitePieces.push_back(piece);
		return &(whitePieces.back());
	}
	else {
		blackPieces.push_back(piece);
		return &(blackPieces.back());
	}

}

void ClassicChess::initClassicGame() {

	whitePieces.reserve(pieceNumber);
	blackPieces.reserve(pieceNumber);

	for (int r{}; r < BOARDROWS; r++) {

		for (int c{}; c < BOARDCOLS; c++) {

			if (r == 1 || r == 6) {
				board[r][c] = storePiece(r, c, Pawn);
			}
			else if (r == 0 || r == 7) {
				if (c == 0 || c == 7) {
					board[r][c] = storePiece(r, c, Rook);
				}
				else if (c == 1 || c == 6) {
					board[r][c] = storePiece(r, c, Knight);
				}
				else if (c == 2 || c == 5) {
					board[r][c] = storePiece(r, c, Bishop);
				}
				else if (c == 3) {
					board[r][c] = storePiece(r, c, Queen);
				}
				else if (c == 4) {
					board[r][c] = storePiece(r, c, King);
					if ((board[r][c])->getColor()) {
						this->whiteKing = board[r][c];
					}
					else {
						this->blackKing = board[r][c];
					}
				}
			}
		};
	};

}

//BOARD/PIECE OPERATIONS ---------------------

// updates the board with a move and returns a move record
ClassicChess::MoveRecord ClassicChess::final_move(const MoveEndpoint& move ) {

			const int ogR = move.p->getRow();
			const int ogC = move.p->getCol();
			const int newR = move.r;
			const int newC = move.c;

			MoveRecord record;
			record.end = move;


			if (move.fashion == EN_PASSENT) {
				record.taken = board[ogR][newC];
			}
			else {
				record.taken = board[newR][newC];
			}

			record.moved = move.p;
			record.startRow = ogR;
			record.startCol = ogC;

			if (move.fashion == EN_PASSENT) {
				if (record.taken) {
					record.taken->captured = true;
				}

				board[ogR][newC] = nullptr; // remove captured pawn
				board[ogR][ogC] = nullptr;  // clear old square

				move.p->incrementMove();
				move.p->move(newR, newC);
				board[newR][newC] = move.p;

				return record;
			}

			if (move.fashion == CASTLE) {
				//do castle
			
				// add or subtract 2 fromt king col
				// rooks new col = king col + or minus 2

				// depends on if rook col > other col

				//idneitfy the rook
				Piece* rookToCastle = board[newR][newC];
				board[newR][newC] = nullptr;

				board[ogR][ogC] = nullptr;

				if (newC > ogC) {
					// rook is to the right of the king
					board[ogR][ogC + 2] = move.p;
					move.p->move(ogR, ogC + 2);
					move.p->incrementMove();

					board[ogR][ogC + 2 - 1] = rookToCastle;
					rookToCastle->move(ogR, ogC + 2 - 1);
					rookToCastle->incrementMove();

				}
				else {
					// rook is to the left of the king
					board[ogR][ogC - 2] = move.p;
					move.p->move(ogR, ogC - 2);
					move.p->incrementMove();

					board[ogR][ogC - 2 + 1] = rookToCastle;
					rookToCastle->move(ogR, ogC - 2 + 1);
					rookToCastle->incrementMove();

				}

				return record;
			}


			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}


			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];

			board[ogR][ogC] = nullptr;

			if (move.fashion == PAWN_PROMOTION) {
				move.p->changeType(PieceType::Queen);
			}

			if (move.fashion == EN_PASSENT) {

				board[ogR][newC] = nullptr;

			};
			
			// auto empties old spot isnt accurate for castling

			return record;
			
		};

// updates the board by undoing a move from its record
void ClassicChess::undo_move(MoveRecord record) {
			
			
			const auto p = record.moved;
			const int newR = record.end.r;
			const int newC = record.end.c;
			const int oldR = record.startRow;
			const int oldC = record.startCol;
			const MoveFashion fashion = record.end.fashion;

			if (fashion == EN_PASSENT) {
				board[newR][newC] = nullptr;

				board[oldR][newC] = record.taken;
				if (record.taken) {
					record.taken->captured = false;
				}


				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();



				return;
			};

			if (fashion == CASTLE && record.taken) {
				//remove the caslte things
				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				// in this case the taken is the rook that moved
				record.taken->move(newR, newC);
				record.taken->deincrementMove();

				// clear the moved spots
				if (newC > oldC) {
					// rook is to the right of the king
					board[oldR][oldC + 2] = nullptr;

					board[oldR][oldC + 2 - 1] = nullptr;

				}
				else {
					// rook is to the right of the king
					board[oldR][oldC - 2] = nullptr;


					board[oldR][oldC - 2 + 1] = nullptr;

				}

				return;
			}

			if (fashion == PAWN_PROMOTION) {

				if (record.taken) {
					record.taken->captured = false;
				}

				p->changeType(Pawn);
				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				return;

			}

			// might laso just encompass undo castle logic
			if (fashion == STANDARD) {

				if (record.taken) {
					record.taken->captured = false;
				}

				board[newR][newC] = record.taken;
				board[oldR][oldC] = p;
				p->move(oldR, oldC);
				p->deincrementMove();

				return;
			}

		}

//MOVE GENERATION------------

//directional vector
static int directions[8][2] = {

		{0, -1},
		{0, 1},
		{-1, 0},
		{1, 0},
		{-1,-1},
		{-1, 1},
		{1,-1}, 
		{1, 1}

	};

//knight movement
static int knightOffsets[8][2] {

		{1, -2},
		{1, 2},
		{-1, -2},
		{-1, 2},
		{2, -1},
		{2, 1},
		{-2, -1},
		{-2, 1}

};

//check if king sqaure is attacked
bool ClassicChess::is_checked(bool is_white) {
	
	// horizontal right
	Piece* king;

	if (is_white) {
		king = whiteKing;
	} else {
		king = blackKing;
	}

	int r = king->getRow();
	int c = king->getCol();
	
	if (is_attacked(r, c, is_white)) {
		return true;
	}

	return false;
}

//check if a certain square is attacked
bool ClassicChess::is_attacked(int r, int c, bool is_white) {
	
	PieceType enemy;

	// check all directions for enemy rook + queen + rook || also check for pawn and king on first tierations
	for (auto vector : directions) {
		
		if ((vector[0] == 0) || (vector[1] == 0)) {
			//straight
			enemy = Rook;
		} else {
			//diagonal
			enemy = Bishop;
		}

		//printBoard();
		int j = 1;

		while (true) {
			// check if out of bounds
			
			int row = r + (vector[0]*j);
			int col = c + (vector[1]*j);

			//std::cout<<j<<row<<col<<"\n";
			
			if (((col) < 0 || (row) < 0) || ((col) > 7|| (row) > 7) ) {
				break;
			}

			auto piece = board[row][col];
				
			if (piece) {
				if (piece->getColor() == is_white) {
					// check if piece is same colors
					break;
						
				} else {
					if (piece->getType() == enemy || piece->getType() == Queen)  {
					
						return true;
					}
					break;
				}
			};

			j++;
		}
	}

	// check for knight 
	for (auto offset : knightOffsets) {
		int row = r + offset[0];
		int col = c + offset[1];

		if (((col) < 0 || (row) < 0) || ((col) > 7 || (row) > 7)) {
			continue;
		}

		auto piece = board[row][col];

		if (piece) {
			if ((piece->getColor() != is_white) && (piece->getType() == Knight)) {
				// check if piece is same colors
			
				return true;

			}
		};
	}

	// check for king and pawn
	//std::cout << "______"<< "\n";
	for (auto vector : directions) {
		
		int row = r + vector[0];
		int col = c + vector[1];

		if (((col) < 0 || (row) < 0) || ((col) > 7 || (row) > 7)) {
			continue;
		}

		
		
		auto piece = board[row][col];
		//check for king
		if (piece) {
			if (piece->getColor() == is_white) {
				// check if piece is same colors
				continue;
			}
			else {
				if (piece->getType() == King) {
					
					return true;
				};
				
				if (piece->getType() == Pawn) {
					
					//piece->toString();
					
					if ((vector[0] != 0) && (vector[1] != 0)) {
						
						
						if (is_white == white_upper) {
							//upper
							
							if (vector[0] > 0) {
								
								return true;
							}

						}
						else {
							//lower
							
							if (vector[0] < 0) {
								
								return true;
							}

						}

					}



				}

			}
				
		}

	}
	return false;

}

//check if its possible for a piece to be a pinned piece
bool ClassicChess::is_pinned(Piece& p) {

	Piece* king = p.getColor() ? whiteKing : blackKing;

	int kingR = king->getRow();
	int kingC = king->getCol();


	if (p.getRow() == kingR) {
		return true;
	}

	if (p.getCol() == kingC) {
		return true;
	}

	if (abs(p.getCol() - p.getRow()) == abs(kingC - kingR)) {
		return true;
	}

	return false;


}

//generate all pseudo legal moves, returns a moveSet, of a piece and its moves
std::vector<ClassicChess::MoveSet> ClassicChess::getPseudoMoves(std::vector<Piece>& pieces) {
	
	std::vector<MoveSet> pseudo_moves;

	for (Piece& p : pieces) {

		if (p.captured) {
			continue;
		}

		MoveSet move;
		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {

			MoveEndpoint e;
			e.r = end[0];
			e.c = end[1];
			e.p = &p;

			// pawn promotion
			if (p.getType() == Pawn && (e.r == 0 || e.r == 7)) {
				e.fashion = PAWN_PROMOTION;
			}
			else {
				e.fashion = STANDARD;
			}

			move.moves.emplace_back(e);

		}

		// castling
		if (p.getType() == King && p.getTimesMoved() == 0) {

			for (auto end : p.getCastledMoves()) {

				MoveEndpoint e;
				e.r = end[0];
				e.c = end[1];
				e.fashion = CASTLE;
				e.p = &p;
				move.moves.emplace_back(e);

			}
		}

		if (p.getType() == Pawn && ((p.getTimesMoved() == 2 || p.getTimesMoved() == 3))) {
			
			for (auto end : p.getEnPassent()) {
				MoveEndpoint e;
				e.r = end[0];
				e.c = end[1];
				e.fashion = EN_PASSENT;
				e.p = &p;
				move.moves.emplace_back(e);
			}

		}

		pseudo_moves.emplace_back(move);
	}
	return pseudo_moves;
}

//calls is_pinned + is_checked on a moveSet to see if it is LEGAL by simulating the move and then undoing it
void ClassicChess::filterMoveSet(ClassicChess::MoveSet& move, bool kingInCheck, Piece* piece) {

	std::array<int,2> start = { piece->getRow() , piece->getCol() };

	for (int i{ static_cast<int>(move.moves.size()) }; --i >= 0;) {

		auto end = move.moves[i];

		//handle check legality
		MoveRecord record = final_move(move.moves[i]);
		bool check = is_checked(piece->getColor());
		undo_move(record);

		

		//handle castle legality
		bool illegal = false;
		if (end.fashion == CASTLE) {
			if (kingInCheck) {
				illegal = true;
			}
			else {
				if (end.c > start[1]) {
					if (is_attacked(start[0], start[1] + 1, piece->getColor())) {
						illegal = true;
					}
				}
				else {
					if (is_attacked(start[0], start[1] - 1, piece->getColor())) {
						illegal = true;
					}
				}
			}
		}

		if (check || illegal) {
			
			move.moves[i] = move.moves.back();
			move.moves.pop_back();

		}

	}


}

//GAME SEQUENCE------------

//checks if there are legal moves available
bool ClassicChess::hasLegalMoves() {
	for (const auto& batch : legalMoves) {
		if (!batch.moves.empty()) {
			return true;
		}
	}
	return false;
}

//calcaultes game state
ClassicChess::OutCome ClassicChess::calculateState()
{
	auto moves = generate_legal_moves(white_move);

	if (!moves.empty())
		return Normal;

	if (is_king_in_check(white_move))
	{
		return white_move ? BlackWin : WhiteWin;
	}

	return Draw;
}

int inline ClassicChess::row_col_to_square(int row, int col)
{
	return row * 8 + col;
}

//validate user input to pick
bool ClassicChess::verifyPick(int r, int c)
{
	if (r < 0 || r >= 8 || c < 0 || c >= 8)
		return false;

	int square = row_col_to_square(r, c);
	uint64_t mask = 1ULL << square;

	if (white_move)
		return w_occupancy & mask;
	else
		return b_occupancy & mask;
}

//validate user input to pick to move
std::variant<bool, ClassicChess::Move> ClassicChess::verifyMove(int from, int to)
{
	auto moves = generate_legal_moves(white_move);

	for (const Move& move : moves)
	{
		if (move.from == from && move.to == to)
		{
			return move;
		}
	}

	return false;
}

//player turn
bool ClassicChess::move_turn()
{
	int req_row;
	int req_col;

	std::cout << "Pick Piece:\n";
	std::cout << (white_move ? "WHITE MOVE\n" : "BLACK MOVE\n");

	std::cout << "row: ";
	std::cin >> req_row;

	std::cout << "col: ";
	std::cin >> req_col;

	while (!verifyPick(req_row, req_col))
	{
		std::cout << "Please pick a valid piece:\n";

		std::cout << "row: ";
		std::cin >> req_row;

		std::cout << "col: ";
		std::cin >> req_col;
	}

	int from = row_col_to_square(req_row, req_col);

	int move_row;
	int move_col;

	std::cout << "Move Piece:\n";

	std::cout << "row: ";
	std::cin >> move_row;

	std::cout << "col: ";
	std::cin >> move_col;

	if (move_row < 0 || move_row >= 8 || move_col < 0 || move_col >= 8)
	{
		std::cout << "Invalid square.\n";
		return false;
	}

	int to = row_col_to_square(move_row, move_col);

	std::variant<bool, Move> moveResult = verifyMove(from, to);

	while (std::holds_alternative<bool>(moveResult))
	{
		std::cout << "Please pick a valid move:\n";

		std::cout << "row: ";
		std::cin >> move_row;

		std::cout << "col: ";
		std::cin >> move_col;

		if (move_row < 0 || move_row >= 8 || move_col < 0 || move_col >= 8)
			continue;

		to = row_col_to_square(move_row, move_col);

		moveResult = verifyMove(from, to);
	}

	Move chosenMove = std::get<Move>(moveResult);

	exec_move(chosenMove);

	return true;
}

//regular pvp gameloop
void ClassicChess::gameLoop()
{
	init_knight_attacks();
	init_king_attacks();
	init_bitboard();

	game = true;
	white_move = true;

	while (game)
	{
		std::cout << "VALUE OF BOARD: " << evaluateBoard() << '\n';

		print_bitboard();

		auto legal = generate_legal_moves(white_move);
		print_moves(legal);

		auto outcome = calculateState();

		std::cout << "outcome: " << outcome << '\n';

		if (outcome != Normal)
		{
			if (outcome == BlackWin)
				std::cout << "Black wins by checkmate\n";
			else if (outcome == WhiteWin)
				std::cout << "White wins by checkmate\n";
			else
				std::cout << "Draw by stalemate\n";

			game = false;
			return;
		}

		bool turn = move_turn();

		if (turn)
		{
			iterator++;
			white_move = !white_move;
		}
	}
}

//ai vs player gameloop
void ClassicChess::gameLoopVSminimaxAI(bool whiteIsAi, int depth) {

	initClassicGame();
	initZobrist();


	while (this->game) {

		std::cout << "VALUE OF BOARD:" << evaluateBoard() << std::endl;
		printBoard();


		uint64_t key = getHashCode(white_move);
		TTEntry& cached = transpositionalTable[key % TTsize];

		bool hasTT = (key == cached.id);

		MoveEndpoint orderTT{};
		if (hasTT) {
			orderTT = cached.move;
		}



		legalMoves = GenerateOrderedLegalMoves(white_move, orderTT, hasTT, depth);
		printAllMoves();

		auto outCome = calculateState();
		std::cout << "outcome : " << outCome << endl;
		if (outCome != Normal) {
			if (outCome == BlackWin) {
				std::cout << "Black wins by checkmate\n";
			}
			else if (outCome == WhiteWin) {
				std::cout << "White wins by checkmate\n";
			}
			else if (outCome == Draw) {
				std::cout << "Draw by stalemate\n";
			}
			game = false;
			return;
		}

		//white always starts

		bool ai_move = (whiteIsAi == white_move);
		if (ai_move) {

			auto bestMove = getBestMoveIterative(depth, white_move);
			final_move(bestMove.move);


			//ai move

		}
		else {

			bool turn = move_turn();



		}

		iterator += 1;
		if (white_move) {
			white_move = false;
		}
		else {
			white_move = true;
		}


		// calgulare the state
	}
}



