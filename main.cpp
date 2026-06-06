// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main() {
    ClassicChess g;

    g.initClassicGame();

    g.real_move(7, 4, 2, 3);
    g.real_move(1, 3, 3, 3);
    g.real_move(1, 4, 5, 6);
    g.printBoard();

    g.generateLegalMoves();
    g.printAllMoves();

    return 0;
}

