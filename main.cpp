// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main(){

	bool game = true;

	ClassicChess g = ClassicChess();
	g.initClassicGame();
	g.printBoard();
	
	g.printWholeMoves();


	while (game) {

	}
	return 0;
	
};

