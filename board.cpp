#include "board.hpp"

// Coords~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Coords::Coords ( int x1_, int y1_, int x2_, int y2_ ) {
    x1 = x1_;
    y1 = y1_;
    x2 = x2_;
    y2 = y2_;
}
bool Coords::operator!= ( const Coords &B ) {
    if ( this -> x1 != B.x1 ||
            this -> y1 != B.y1 ||
            this -> x2 != B.x2 ||
            this -> y2 != B.y2 )
        return true;
    return false;
}

// Moves~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Move::Move( int x1, int y1, int x2, int y2 ) {
    coords.emplace_back( x1, y1, x2, y2 );
}
Move::Move( Coords c ) {
    coords.push_back( c );
}
void Move::add( Coords c ) {
    coords.insert( coords.begin(), c );
}
bool Move::operator== ( const Move &B ) {
    if ( this -> coords.size() != B.coords.size() )
        return false;
    else
        for ( int i = 0; i < coords.size(); i++ )
            if ( this -> coords[i] != B.coords[i] )
                return false;
    return true;
}

// Board~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Board::Board() {
    reset();
}

void Board::reset() {
    drawCounter == 0;
    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            if ( i%2 != j%2 ) {
                if ( i < 3 )
                    board[i][j] = RED_PAWN;
                else if ( i > 4 )
                    board[i][j] = BLACK_PAWN;
                else
                    board[i][j] = EMPTY;
            } else
                board[i][j] = EMPTY;
        }
    }
}

bool Board::checkCapture( int x, int y ) {
    // red 0, black 1
    bool color = ( board[x][y] == RED_PAWN || board[x][y] == RED_KING ) ? 0 : 1;
    // check for pawns
    if ( board[x][y] == RED_PAWN || board[x][y] == BLACK_PAWN ) {
        for ( int a : {-1, 1} ) {
            for ( int b : {-1, 1} ) {
                int i = x + a;
                int j = y + b;
                if ( i+a < 8 && i+a >= 0 && j+b < 8 && j+b >= 0 ) {
                    if ( color ) {
                        if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                            if ( board[i+a][j+b] == EMPTY )
                                return true;
                    } else {
                        if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN )
                            if ( board[i+a][j+b] == EMPTY )
                                return true;
                    }
                }
            }
        }
    }
    // check for kings
    else if ( board[x][y] == RED_KING || board[x][y] == BLACK_KING ) {
        for ( int a : {-1, 1} ) {
            for ( int b : {-1, 1} ) {
                int i = x + a;
                int j = y + b;
                while ( i+a < 8 && i+a >= 0 && j+b < 8 && j+b >= 0 ) {
                    if ( color ) {
                        if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN )
                            break;
                        if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN ) {
                            if ( board[i+a][j+b] == EMPTY )
                                return true;
                            else
                                break;
                        }
                    } else {
                        if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                            break;
                        if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN ) {
                            if ( board[i+a][j+b] == EMPTY )
                                return true;
                            else
                                break;
                        }
                    }
                    i += a;
                    j += b;
                }
            }
        }
    }
    return false;
}

bool Board::checkCapture( bool color ) {
    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            if ( color ) {
                if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN )
                    if ( checkCapture( i, j ) )
                        return true;
            } else {
                if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                    if ( checkCapture( i, j ) )
                        return true;
            }
        }
    }
    return false;
}

// get all viable sequences after first move
std::vector< Move > Board::getMoveSequences( Move move, Board b ) {
    std::vector< Move > moves;
    if ( b.move( move.coords[0] ) == 1 ) {
        moves.emplace_back( move.coords[0] );
    } else {
        for ( auto& m : b.getViableMoves( move.coords[0].x2, move.coords[0].y2 ) ) {
            std::vector< Move > tmp = getMoveSequences( m, b );
            moves.insert( moves.end(), tmp.begin(), tmp.end() );
        }
        for ( auto& m : moves ) {
            m.add( move.coords[0] );
        }
        return moves;
    }
}

// get all viable first moves for a color
std::vector< Move > Board::getViableMoves( bool color ) {
    std::vector< Move > moves;
    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            if ( color ) {
                if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN ) {
                    auto tmp = getViableMoves( i, j );
                    for ( auto& m : tmp ) {
                        auto tmp2 = getMoveSequences( m, *this );
                        moves.insert( moves.end(), tmp2.begin(), tmp2.end() );
                    }
                }
            } else {
                if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN ) {
                    auto tmp = getViableMoves( i, j );
                    for ( auto& m : tmp ) {
                        auto tmp2 = getMoveSequences( m, *this );
                        moves.insert( moves.end(), tmp2.begin(), tmp2.end() );
                    }
                }
            }
        }
    }
    return moves;
}

// get all viable first moves for a pawn
std::vector< Move > Board::getViableMoves( int x, int y ) {
    bool color;
    std::vector< Move > moves;
    // if a field is empty return empty vector
    if ( board[x][y] == EMPTY )
        return moves;
    if ( board[x][y] == RED_PAWN || board[x][y] == RED_KING ) {
        color = 0;
    } else {
        color = 1;
    }
    // If a capture is possible on the other move just return empty vector
    if ( checkCapture( color ) != checkCapture( x, y ) )
        return moves;
    // Check for pawns
    if ( board[x][y] == RED_PAWN || board[x][y] == BLACK_PAWN ) {
        // If there are no capturing opportunities
        if ( !checkCapture( x, y ) ) {
            int xdir = color ? -1 : 1;
            if ( y < 7 )
                if ( board[x+xdir][y+1] == EMPTY )
                    moves.push_back( Move( x, y, x+xdir, y+1 ) );
            if ( y > 0 )
                if ( board[x+xdir][y-1] == EMPTY )
                    moves.push_back( Move( x, y, x+xdir, y-1 ) );
        // If there are captures available
        } else {
            for ( int a : {-1, 1} ) {
                for ( int b : {-1, 1} ) {
                    int i = x + a;
                    int j = y + b;
                    if ( i+a < 8 && i+a >= 0 && j+b < 8 && j+b >= 0 ) {
                        if ( color ) {
                            if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                                if ( board[i+a][j+b] == EMPTY )
                                    moves.push_back( Move( x, y, i+a, j+b ) );
                        } else {
                            if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN )
                                if ( board[i+a][j+b] == EMPTY )
                                    moves.push_back( Move( x, y, i+a, j+b ) );
                        }
                    }
                }
            }
        }
    // Check for Kings
    } else {
        // flag if the move is a capture
        bool captureFlag = checkCapture( x, y );
        // for each direction
        for ( int a : {-1, 1} ) {
            for ( int b : {-1, 1} ) {
                int i = x + a;
                int j = y + b;
                // move until there is an obstacle on a path
                while ( i >= 0 && j >= 0 && i <= 7 && j <= 7 ) {
                    //if not capturing add those moves
                    if ( board[i][j] == EMPTY ) {
                        if ( !captureFlag )
                            moves.push_back( Move( x, y, i, j ) );
                    } else if ( color ) {
                        // break if an ally on path
                        if ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN )
                            break;
                        // add all moves possible after a capture
                        else if ( ( board[i][j] == RED_KING || board[i][j] == RED_PAWN ) && captureFlag ) {
                            do {
                                i += a;
                                j += b;
                                if ( i >= 0 && j >= 0 && i <= 7 && j <= 7 ) {
                                    if ( board[i][j] == EMPTY )
                                        moves.push_back( Move( x, y, i, j ) );
                                    else
                                        break;
                                } else
                                    break;
                            } while ( true );
                            break;
                        } else
                            break;
                    } else {
                        // break if an ally on path
                        if ( board[i][j] == RED_KING || board[i][j] == RED_PAWN )
                            break;
                        // add all moves possible after a capture
                        if ( ( board[i][j] == BLACK_KING || board[i][j] == BLACK_PAWN ) && captureFlag ) {
                            do {
                                i += a;
                                j += b;
                                if ( i >= 0 && j >= 0 && i <= 7 && j <= 7 ) {
                                    if ( board[i][j] == EMPTY )
                                        moves.push_back( Move( x, y, i, j ) );
                                    else
                                        break;
                                } else
                                    break;
                            } while ( true );
                            break;
                        } else
                            break;
                    }
                    i += a;
                    j += b;
                }
            }
        }
    }
    return moves;
}

