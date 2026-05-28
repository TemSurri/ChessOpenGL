// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main(){

	ClassicChess g = ClassicChess();
	g.initClassicGame();
	g.printBoard();


	
	g.whitePieces[1].toString();
	g.generateLegalMoves();
	g.printAllMoves();



	g.move_turn();
	g.printBoard();
	
	
	return 0;
	
};

