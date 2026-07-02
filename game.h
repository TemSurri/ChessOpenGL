#pragma once
#include <vector>
#include "piece.h"
#include <array>
#include <variant>

// to do organize code to only need to use one array, and is turn thats all

class ClassicChess {

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw,
		Normal
	};

	// stores ptr to piece and all its moves
	struct MoveSet {
		
		Piece* piece;
		std::vector<MoveEndpoint> moves;
		
	};

	struct EvaluatedMove {
		// value of minimax after it was searched
		int value;

		Piece* piece = nullptr;
		MoveEndpoint move;

	};

	struct MoveRecord {

		Piece* taken = nullptr;
		Piece* moved = nullptr;
		int startRow;
		int startCol;
		MoveEndpoint end;

	};

	private:
		// init 
		bool game = true;
		bool white_upper = true;

		// to change every move
		bool white_move = true;
		int iterator{};

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};

		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		// stores only moveable pieces and their respective moves
		std::vector<MoveSet> legalMoves{};

		// actually store pieces
		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;

		//all board manipulation
		MoveRecord final_move(Piece* p, const MoveEndpoint& move);
		void undo_move(MoveRecord record);

		//setup
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();

		//move generation need to pass in turn for context
		bool is_checked(bool is_white);
		bool check(bool for_white);
		// checks if quadrant is attacked
		bool is_attacked(int r, int c, bool is_white);

		void generateLegalMoves(bool is_white);

		std::vector<MoveSet> getPseudoMoves(std::vector<Piece>& pieces);
	
		// piece aready passed in these 3 so no need to pass in turn
		void filterMoveSet(MoveSet& move, bool kingInCheck);
		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);

		//Game Logic
		OutCome calculateState();
		bool move_turn();
		bool verifyPick(int r, int c);
		std::variant<bool, MoveEndpoint> verifyMove(int r, int c, Piece* piece);


	public:

		ClassicChess() {
			
		};

		//move
		void printAllMoves();
		void printBoard();

		void gameLoop();


		
		//minimax 
		
		int evaluateBoard();
		EvaluatedMove getBestMove(int depth, bool maximizing);
		int minimax(int depth, bool maximizing);
		const int whiteMaximizing = true;
		std::vector<MoveSet> ClassicChess::generateLegalMovesNode(bool is_white);
		
};