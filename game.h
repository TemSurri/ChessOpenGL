#pragma once
#include <vector>
#include "piece.h"
#include <array>

class ClassicChess {


	struct BoardState {
		bool draw = false;

		bool whiteCheckMated = false;
		bool blackCheckMated = false;

		bool whiteChecked = false;
		bool blackChecked = false;

		bool normal = false;
		bool active = true;
	};

	enum OutCome {
		WhiteWin,
		BlackWin,
		Draw
	};

	// stores ptr to piece and all its moves
	struct MoveSet {
		
		Piece* piece;
		std::vector<MoveEndpoint> moves;
		
	};

	private:
		BoardState game;
		bool white_upper = true;
		int iterator{};

		// to change every move
		bool white_move = true;

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};

		Piece* blackKing{ nullptr };
		Piece* whiteKing{ nullptr };

		// stores only moveable pieces and their respective moves
		std::vector<MoveSet> legalBlackMoves{};
		std::vector<MoveSet> legalWhiteMoves{};

		

	public:
		//public for now 

		std::vector<Piece> whitePieces;
		std::vector<Piece> blackPieces;
		//----------------

		ClassicChess() {
			
		};

		//helper functions
		void printBoard() {
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
		};

		
		void real_move(int ogR, int ogC, int newR, int newC) {
			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}

			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];

			board[ogR][ogC] = nullptr;
			// auto empties old spot isnt accurate for castling
		};

		void final_move(Piece& p, MoveEndpoint move ) {
			int ogR = p.getRow();
			int ogC = p.getCol();
			int newR = move.r;
			int newC = move.c;


			if (board[newR][newC]) {
				board[newR][newC]->captured = true;
			}

			board[ogR][ogC]->incrementMove();
			board[ogR][ogC]->move(newR, newC);
			board[newR][newC] = board[ogR][ogC];


			board[ogR][ogC] = nullptr;
			if (move.fashion == PAWN_PROMOTION) {
				p.changeType(Queen);
			}
			if (move.fashion == CASTLE) {
				//do castle
				std::cout << "do castle move";
			}
			
			// auto empties old spot isnt accurate for castling
		};

		

		void printMoves(vector<array<int, 2>> list) {

			for (int i{}; i < list.size(); i++) {
				cout << "(" << list[i][0] << ", " << list[i][1] << ") " << endl;
			}
		}

		void printAllMoves();
		//setup
		Piece* storePiece(int r, int c, PieceType type);
		void initClassicGame();


		//move generation
		bool is_checked(bool is_white);
		bool check(bool for_white);
		void generateLegalMoves();

		std::vector<MoveSet> getBlackPseudoMoves();
		std::vector<MoveSet> getWhitePseudoMoves();
	
		void filterMoveSet(MoveSet& move);

		bool is_pinned(Piece& p);
		bool virtualMoveCauseCheck(MoveSet move);
		BoardState calculateState();
		OutCome gameLoop();
		BoardState move( bool white );
		bool move_turn();
		bool verifyPick(int r, int c);
		bool verifyMove(int r, int c, Piece* piece);

		// everything should end up private escpet the final startGame()


};