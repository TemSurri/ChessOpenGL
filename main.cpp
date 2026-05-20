// main.cpp : Defines the entry point for the application.
//

#include "game.h"



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

