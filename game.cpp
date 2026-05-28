#include "game.h"
#include "pieceInfo.h"
#include "vector"
#include <iostream>

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

ClassicChess::BoardState ClassicChess::calculateState() {
	//calculate State

	BoardState boardState;
	
	for (auto move : this->whiteMoves){

		if ((move.move[1][0] == blackKing->getRow()) && (move.move[1][1] == blackKing->getCol())){
			boardState.blackChecked = true;
			if (blackMoves.size() == 0){
				boardState.blackCheckMated = true;
			}
		} 
	}

	for (auto move : this->blackMoves){

		if ((move.move[1][0] == whiteKing->getRow()) && (move.move[1][1] == whiteKing->getCol())){
			boardState.whiteChecked = true;
			if (whiteMoves.size() == 0){
				boardState.whiteCheckMated = true;
			}

		}
	}


	if (((whiteMoves.size() == 0) && (blackMoves.size() == 0)) && iterator !=0) {
		boardState.draw = true;
	}

	return boardState;
	// 2

};

//a function that moves the board and then undoes the move
ClassicChess::BoardState ClassicChess::virtualMove(MoveInfo move) {

	// 1


	auto start = move.move[0];
	auto end = move.move[1];

	Piece* taken = board[end[0]][end[1]];

	//make move
	this->move(move);
	//get state
	BoardState state = calculateState();

	//undo move
	board[end[0]][end[1]] = taken;
	board[start[0]][start[1]] = move.piece;
	move.piece->move(start[0], start[1]);
		
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

			//filter white moves by check
			
			BoardState states = virtualMove(move);
			if (states.whiteChecked) {
				continue;
			} else {
				white_moves.emplace_back(move);
			}

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


			BoardState states = virtualMove(move);
			if (states.blackChecked) {
				continue;
			} else {
				black_moves.emplace_back(move);
			}

		}
	}



	//link to main 
	this->blackMoves = black_moves;
	this->whiteMoves = white_moves;

}

bool ClassicChess::verifyPick(int r, int c){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}

	if (board[r][c]) {
		if (board[r][c]->getColor() == white_move)
		{
			return true;
		}
	} 
	
	return false;

}

bool ClassicChess::verifyMove(int r, int c, Piece* piece){

	if ((r>=8 || r < 0) || (c>=8 || c < 0)) {
		return false;
	}


	std::cout<<"exists";
	if (white_move) {
		for (auto move: this->whiteMoves) {

			if (move.piece == piece) {
				std::cout<<"macthes";
				auto coords = move.move[1];
				std::cout<<r<<c<<coords[0]<<coords[1]<<std::endl;					
				if ((coords[0] == r) && (coords[1] == c)) {
					return true;
				}
			} 

		}
	}
	
	
	return false;

}

ClassicChess::BoardState ClassicChess::move_turn() {

	int req_row;
	int req_col;

	std::cout<<"Pick Piece:"<<std::endl;

	std::cout<<"row : ";
	std::cin>>req_row;

	std::cout<<"col : ";
	std::cin>>req_col;

	bool pick = verifyPick(req_row, req_col);
	while (!pick) {

		std::cout<<"please Pick a valid Piece:"<<std::endl;

		std::cout<<"row : ";
		std::cin>>req_row;

		std::cout<<"col : ";
		std::cin>>req_col;

		pick = verifyPick(req_row, req_col);
		
	}

	Piece* piece = ClassicChess::board[req_row][req_col];
	// pick works

	int move_row;
	int move_col;

	std::cout<<"Move Piece:"<<std::endl;

	piece->toString();

	std::cout<<"row : ";
	std::cin>>move_row;

	std::cout<<"col : ";
	std::cin>>move_col;

	bool move = verifyMove(move_row, move_col, piece);
	while (!move) {

		std::cout<<"Please pick a valid move:"<<std::endl;

		std::cout<<"row : ";
		std::cin>>move_row;

		std::cout<<"col : ";
		std::cin>>move_col;
		
		move = verifyMove(move_row, move_col, piece);
	}


	//happens if eveyrhing is succesful
	this->move(req_row, req_col, move_row, move_col);


};




