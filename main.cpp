// main.cpp : Defines the entry point for the application.
//

#include "gameInfo.h"
#include "pieceInfo.h"
#include <vector>
#include <array>
#include "game.h"

using namespace std;

Piece* board[BOARDROWS][BOARDCOLS]{ nullptr }; //change this to board with objects
std::vector<Piece> whitePieces;
std::vector<Piece> blackPieces;

int main(){

	bool game = true;

	ClassicChess g = ClassicChess();
	g.initClassicGame();
	g.printBoard();
	
	g.whitePieces[4].toString();
	g.printMoves(g.whitePieces[4].pseudoLegalMoves());

	g.move(0, 5, 3, 3);
	g.printMoves(g.whitePieces[4].pseudoLegalMoves());

	g.printBoard();


	while (game) {

	}
	return 0;
	
};

