#include "game.h"
#include "gameInfo.h"
#include <array>

// in the index order of my enum in 
constexpr std::array<int, 7> pieceValues = {
    0,
    100,
    320,
    330,
    500,
    900,
    20000
};

std::vector<ClassicChess::MoveSet> ClassicChess::generateLegalMovesNode(bool is_white) {

    bool isChecked = is_checked(is_white);
    auto pmoves = is_white ? (getPseudoMoves(whitePieces)) : (getPseudoMoves(blackPieces));

    std::vector<MoveSet> legalMoves;

    for (int i{}; i < pmoves.size(); i++) {

        if (pmoves[i].moves.size() == 0) {
            continue;
        }

        if (isChecked || is_pinned((*pmoves[i].piece))) {
            filterMoveSet(pmoves[i], isChecked);
        }

        if (pmoves[i].moves.size() > 0) {
            legalMoves.push_back(pmoves[i]);
        }
    }

    return legalMoves;
}

int ClassicChess::evaluateBoard() {
    int boardValue{};

    for (auto& p : whitePieces) {
        if (p.captured) {
            continue;
        }
        boardValue = (whiteMaximizing) ? (boardValue + pieceValues[p.getType()]) : (boardValue - pieceValues[p.getType()]);
    }

    for (auto& p : blackPieces) {
        if (p.captured) {
            continue;
        }
        boardValue = (whiteMaximizing) ? (boardValue - pieceValues[p.getType()]) : (boardValue + pieceValues[p.getType()]);
    }

    return boardValue;
}

// big issue array circular dependence
ClassicChess::EvaluatedMove ClassicChess::getBestMove(int depth, bool whiteToMove) {
    // moves already generated
    EvaluatedMove alpha;
    bool maximizing = (whiteToMove == whiteMaximizing);
    if (maximizing) {
        alpha.value = -100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int beta = minimax(depth - 1, !whiteToMove);
                undo_move(mrecord);

                if (beta >= alpha.value) {
                    alpha = {
                        beta, move.piece, endpoint
                    }; 
                }

            }
        }

    }

    else {
        alpha.value = 100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int beta = minimax(depth - 1, !whiteToMove);
                undo_move(mrecord);

                if (beta <= alpha.value) {
                    alpha = {
                        beta, move.piece, endpoint
                    };
                }

            }
        }

    }

    return alpha;

}
// no alpha beta pruning yet, it will reuire another paramter which is the alpha.value of the higher layer
int ClassicChess::minimax(int depth, bool whiteToMove) {
    
    if (depth == 0) {
        return evaluateBoard();
    }

   
    //generate moves
    // it will generate the moves for the side that is max or min depending on input
    auto legal_moves_node = generateLegalMovesNode(whiteToMove);
    int alpha;

    bool maximizing = (whiteToMove == whiteMaximizing);

    if (maximizing) {
        if (legal_moves_node.empty()) {
            if (is_checked(whiteToMove)) {
                return -100000; //checkmated
            }
            return 0; // stalemate
        }

        //alpha = vsmall#
        alpha = -100000;
        //for move in moves
        //  do move
        //  beta = minimax(move, p, depth-1, !maximizing)
        //  undo move
        //  if beta is larger than alpha, beta is new alpha
        
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int beta = minimax(depth - 1, !whiteToMove);
                undo_move(mrecord);

                if (beta >= alpha) {
                    alpha = beta;
                }
            }
        }

        

    }
    else {
        //alpha = vbig#
        // basicaly same as top

        if (legal_moves_node.empty()) {
            if (is_checked(whiteToMove)) {
                return 100000; //checkmated
            }
            return 0; // stalemate
        }

        alpha = 100000;
       
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int beta = minimax(depth - 1, !whiteToMove);
                undo_move(mrecord);

                if (beta <= alpha) {
                    alpha = beta;
                }
            }
        }

       

    }
   
    return alpha;
}

