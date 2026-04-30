#include <iostream>
#include <vector>
#include "gameInfo.h"

using namespace std;

class Piece {
private:
	int x{};
	int y{};
	bool is_white;
	int type;
public:
	Piece(int x, int y, bool white, PieceType type)
		: x{ x }, y{ y }, is_white{ white }, type{ type } {
	};

	void move(int x, int y) {
		this->y = y;
		this->x = x;
	};

	void changeType(PieceType type) {
		type = type;
	}

	int getType() {
		return type;
	}

	bool getColor() {
		return is_white;
	}

};
// map of board with pointers to the pieces for quick search up
extern Piece* board[BOARDROWS][BOARDCOLS];

// list of pieces to keep track of
extern std::vector<Piece> whitePieces;
extern std::vector<Piece> blackPieces;