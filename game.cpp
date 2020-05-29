#include "board.hpp"

// capture pawn or king from between those coords
void Board::capture( int x1, int y1, int x2, int y2 ) {
    // mark direction
    int a = (x1<x2)? 1 : -1;
    int b = (y1<y2)? 1 : -1;
    // for all fields through the way
    for ( int i = x1+a, j = y1+b; i != x2, j != y2; i+=a, j+=b ) {
        // clear it
        board[i][j] = EMPTY;
    }
}

// returns 0 if wrong move, 1 if finished, and 2 if there is still a capture to do
int Board::move( Coords m ) {
    bool color = (board[m.x1][m.y1] == RED_KING || board[m.x1][m.y1] == RED_PAWN) ? 0 : 1;
    return move( m.x1, m.y1, m.x2, m.y2, color );
}

int Board::move( Move m ) {
    bool color = ( board[m.coords[0].x1][m.coords[0].y1] == RED_KING ||
                    board[m.coords[0].x1][m.coords[0].y1] == RED_PAWN ) ? 0 : 1;
    int i{};
    while ( move( m.coords[i] ) == 2 )
        i++;
    return 1;
}

int Board::move( int x1, int y1, int x2, int y2, bool color ) {
    // check if player moved right color
    if ( ( color && (board[x1][y1] == RED_KING || board[x1][y1] == RED_PAWN) ) ||
        ( !color && (board[x1][y1] == BLACK_KING || board[x1][y1] == BLACK_PAWN) ) )
        return 0;

    bool captureFlag = false;
    // if given move is in viable moves for that pawn/king
    for ( auto& move : getViableMoves( x1, y1 ) ) {
        if ( move == Move( x1, y1, x2, y2 ) ) {
            // if a move included capture
            if ( checkCapture( color ) ) {
                capture( x1, y1, x2, y2 );
                captureFlag = true;
            }
            board[x2][y2] = board[x1][y1];
            board[x1][y1] = EMPTY;
            // if there is still a capture for this pawn/king
            if ( captureFlag && checkCapture( x2, y2 ) )
                return 2;
            // if a move ended in a promote
            else if ( ( !color && x2 == 7) || ( color && x2 == 0 ) )
                board[x2][y2] = color ? BLACK_KING : RED_KING;
            return 1;
        }
    }
    return 0;
}

// returns 0 if game continues 1 if red won, 2 if black won, -1 if tie
int Board::checkState( bool color ) {
    int redPawns{}, blackPawns{}, redKings{}, blackKings{};
    bool redMoves{}, blackMoves{};
    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            switch ( board[i][j] ) {
                case EMPTY:
                    break;
                case RED_PAWN:
                    if ( !redMoves )
                        if ( getViableMoves( i, j ).size() != 0 )
                            redMoves = true;
                    redPawns++;
                    break;
                case BLACK_PAWN:
                    if ( !blackMoves )
                            if ( getViableMoves( i, j ).size() != 0 )
                                blackMoves = true;
                    blackPawns++;
                    break;
                case RED_KING:
                    if ( !redMoves )
                        if ( getViableMoves( i, j ).size() != 0 )
                            redMoves = true;
                    redKings++;
                    break;
                case BLACK_KING:
                    if ( !blackMoves )
                        if ( getViableMoves( i, j ).size() != 0 )
                            blackMoves = true;
                    blackKings++;
                    break;
            }
        }
    }
    if ( ( blackPawns || blackKings ) && ( redPawns || redKings ) && ( ( color && blackMoves ) || ( !color && redMoves ) ) ) {
        if ( blackKings && redKings &&
                blackPawns == blackP &&
                redPawns == redP &&
                blackKings == blackK &&
                redKings == redK ) {
            if ( ++drawCounter == 30 )
                return -1;
            else
                drawCounter = 0;
        }
        blackP = blackPawns;
        redP = redPawns;
        blackK = blackKings;
        redK  = redKings;
        return 0;
    }
    else if ( ( !blackPawns && !blackKings ) || ( color && !blackMoves ) )
        return 1;
    else if ( ( !redPawns && !redKings ) || ( !color && !redMoves ) )
        return 2;
}

// evaluate value of a field
// pawn 7 points
// last row + 0
// semi last row + 1
// 3rd row + 2
// > 4th row + 3
// king 25 points
// anything almost connected with left or right border + 1 point
// directly connected +2
// capture oportunity + 15
int Board::evaluateField( int x, int y ) {
    int score{};
    switch( board[x][y] ) {
        case EMPTY:
            break;
        case RED_PAWN:
            score += 7;
            if ( x > 0 ) {
                score += 2;
                if ( x > 1 ) {
                    score ++;
                    if ( x > 2 )
                        score ++;;
                }
            }
            if ( y < 2 || y > 5 ) {
                score ++;
                if ( y < 1 || y > 6 )
                    score ++;
            }
            score += 15;
            break;
        case BLACK_PAWN:
            score += 7;
            if ( x < 7 ) {
                score += 2;
                if ( x < 6 ) {
                    score ++;
                    if ( x < 5 )
                        score ++;
                }
            }
            if ( y < 2 || y > 5 ) {
                score ++;
                if ( y < 1 || y > 6 )
                    score ++;
            }
            score += 15;
            break;
        case RED_KING:
            score += 25;
            if ( y < 2 || y > 5 ) {
                score ++;
                if ( y < 1 || y > 6 )
                    score ++;
            }
            score += 15;
            break;
        case BLACK_KING:
            score += 25;
            if ( y < 2 || y > 5 ) {
                score ++;
                if ( y < 1 || y > 6 )
                    score ++;
            }
            score += 15;
            break;
    }
    return score;
}

// returns value depending on game state and sum of field values
int Board::evaluateBoard( bool color, bool side ) {
    switch ( checkState( color ) ) {
        case -1:
            return 0;
        case 0:
            break;
        case 1:
            return side ? -10000 : 10000;
        case 2:
            return side ? 10000 : -10000;
    }
    int black{}, red{};
    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                red += evaluateField( i, j );
            else if ( board[i][j] != EMPTY )
                black += evaluateField( i, j );
        }
    }
    if ( side )
        return black - red;
    else
        return red - black;
}
