// main.cpp : Defines the entry point for the application.
//

#include "game.h"



int main() {
    ClassicChess g;

    g.gameLoopVSminimaxAI(true, 7); 

    return 0;
}

