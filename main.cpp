// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main() {
    ClassicChess g;

    g.gameLoopVSminimaxAI(true, 6); 
    //g.initClassicGame();


    //g.real_move(6, 3, 3, 3);
    //g.real_move(1, 4, 5, 4);
    //g.printBoard();

   // g.generateLegalMoves();
	//g.printAllMoves();


    return 0;
}

