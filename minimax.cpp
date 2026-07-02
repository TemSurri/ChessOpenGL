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

    int alpha = -100000;
    int beta = 100000;


    EvaluatedMove best;
    bool maximizing = (whiteToMove == whiteMaximizing);
    if (maximizing) {
        best.value = -100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);

                if (cur_val > best.value) {
                    best = {
                        cur_val, move.piece, endpoint
                    }; 
                }


                if (best.value > alpha) {
                    alpha = best.value;
                }

            }
        }

    }

    else {
        best.value = 100000;
        for (auto& move : legalMoves) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);

                undo_move(mrecord);

                if (cur_val < best.value) {
                    best = {
                        cur_val, move.piece, endpoint
                    };
                }

                if (best.value < beta) {
                    beta = best.value;
                }

            }
        }

    }

    return best;

}

int ClassicChess::minimax(int depth, bool whiteToMove, int alpha, int beta) {
    
    if (depth == 0) {
        return evaluateBoard();
    }

   
    //generate moves
    // it will generate the moves for the side that is max or min depending on input
    auto legal_moves_node = generateLegalMovesNode(whiteToMove);
    int best;

    bool maximizing = (whiteToMove == whiteMaximizing);

    if (maximizing) {
        if (legal_moves_node.empty()) {
            if (is_checked(whiteToMove)) {
                return -100000; //checkmated
            }
            return 0; // stalemate
        }

        //alpha = vsmall#
        best = -100000;
        //for move in moves
        //  do move
        //  beta = minimax(move, p, depth-1, !maximizing)
        //  undo move
        //  if beta is larger than alpha, beta is new alpha
        
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {

                MoveRecord mrecord = final_move(move.piece, endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);


                if (cur_val > best) {
                    best = cur_val;
                }           

                if (best > alpha) {
                    alpha = best;
                }

                if (alpha >= beta) {
                    return best;
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

        best = 100000;
       
        for (auto& move : legal_moves_node) {
            for (auto& endpoint : move.moves) {
                MoveRecord mrecord = final_move(move.piece, endpoint);
                int cur_val = minimax(depth - 1, !whiteToMove, alpha, beta);
                undo_move(mrecord);

                if (cur_val < best) {
                    best = cur_val;
                }

                if (best < beta) {
                    beta = best;
                }

                if (beta <= alpha) {
                    return best;
                }


            }
        }

       

    }
   
    return best;
}

