#include "game.h"
#include "pieceInfo.h"
#include "vector"

Piece* ClassicChess::storePiece(int r, int c, PieceType type) {

	// assume row greater than 4 is white
	// this is where we can store differnt objects based on type but for not itll be on Piece class later can do inhertiance

	bool upper = r < BOARDROWS / 2;
	bool is_white = (upper == white_upper);

	Piece piece = Piece(r, c, is_white, type, board);

	if (type == PieceType::King) {

		if (is_white) {
			this->whiteKing = &piece;
		}
		else if (!is_white) {
			this->blackKing = &piece;
		}
	}

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
				}
			}
		};
	};

}

ClassicChess::States ClassicChess::calculateState() {
	//calculate State

	// 2

};

//a function that moves the board and then undoes the move
ClassicChess::States ClassicChess::virtualMove(MoveInfo move) {

	// 1


	auto start = move.move[0];
	auto end = move.move[1];

	Piece* taken = board[end[0]][end[1]];

	//make move
	this->move(move);
	//get state
	States state = calculateState();

	//undo move
	board[end[0]][end[1]] = taken;
	board[start[0]][start[1]] = move.piece;
		
	//return state
	return state;

}

void ClassicChess::generateLegalMoves() {

	// get all pseudo White moves

	std::vector<MoveInfo> white_moves;

	for (Piece& p : whitePieces) {
		std::array<int, 2> start_pos = { p.getRow(), p.getCol() };


		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			MoveInfo move;
			move.piece = &p;
			move.move = { start_pos, end };

			white_moves.emplace_back(move);
		}
	}


	// get all pseudo Black moves

	std::vector<MoveInfo> black_moves;

	for (Piece& p : blackPieces) {
		std::array<int, 2> start_pos = { p.getRow(), p.getCol() };


		auto moves = p.pseudoLegalMoves();

		for (auto end : moves) {
			MoveInfo move;
			move.piece = &p;
			move.move = { start_pos, end };

			black_moves.emplace_back(move);
		}
	}

	//filter white moves by check
	// 0
	//filter black mvoes by check
	// 0
}


//ClassicChess::States ClassicChess::move(bool white) {
	//if (white) {
		//white move

		//get moves
		//get input
		//validate input
		//return state



	//}
	//else {
		//black move

		//get moves
		//get input
		//validate input
		//return state



	//}
//};



