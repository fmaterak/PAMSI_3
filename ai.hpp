#ifndef AI_HPP_
#define AI_HPP_
#include <vector>
#include <memory>
#include "board.hpp"

// self generating tree that stores possible board states n-moves ahead
class Node {
    public:
    bool color;
    Board board;
    int value;
    bool side;
    Move move;
    std::vector< Node > childs{};

    Node( int depth, Board board_, bool side_, bool color_, Move move_ = Move(0,0,0,0) );
};

int max( int a, int b );

int min( int a, int b );
// chooses the best move according to value
int minimax( Node& node, int depth, bool isMaximizingPlayer, int alpha, int beta );

class AI {
    AI(){}
    public:
    static Move getBestMove( int maxdepth, Board board, bool side );
};

#endif