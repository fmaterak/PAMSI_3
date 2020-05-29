#ifndef Checkers_HPP
#define Checkers_HPP

#include <unistd.h>
#include <SFML/Graphics.hpp>
#include "board.hpp"
#include "ai.hpp"

const int size = 850;

class Checkers {
    // 0 human 1 AI
    bool player;
    bool side;

    sf::RenderWindow win;
    sf::Texture textures[6];
    sf::Sprite sprites[6];

    Board board;

    public:
    // set up textures & sprites and the starting player
    Checkers();
    // main game loop
    void play();
    // starting game sequence
    bool gameStart();
    // ending game sequence
    int gameOver( int state );
    // player move
    void playerMove();
    // if arg is true it plays other color
    void cpuMove( bool second = false );
    // window closing
    bool handleEvents();
    // mouse handling
    int mouse();
    // drawing
    void draw();
};

#endif
