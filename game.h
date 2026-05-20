#pragma once
#include <vector>
#include "pieceInfo.h"

class ClassicChess {

	enum States {

		Draw,

		WhiteCheckMated,
		BlackCheckMated,

		WhiteChecked,
		BlackChecked,

		BlackMove,
		WhiteMove,

	};

	private:
		States game;
		bool white_upper = true;

		Piece* board[BOARDROWS][BOARDCOLS]{nullptr};


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
		};

		void move(int ogR, int ogC, int newR, int newC) {
			Piece* p = board[ogR][ogC];
			p->toString();
			board[newR][newC] = p;
			p->move(newR, newC);
			p->toString();

			board[ogR][ogC] = nullptr;
			// auto empties old spot isnt accurate for castling
		};

		void printMoves(vector<array<int, 2>> list) {

			for (int i{}; i < list.size(); i++) {
				cout << "(" << list[i][0] << ", " << list[i][1] << ") " << endl;
			}
		}


		Piece* storePiece(int r, int c, PieceType type);

		void initClassicGame();
		States calculateState();
		

		// everything hsould end up private escpet the final startGame()


};