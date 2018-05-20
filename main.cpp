#include "Game.h"


int main(int argc, char **argv)
{
    if (2 != argc) {
        /* TODO: Usage. */
    }
    
    (void) argv;
    
    //std::string game_type(argv[1]);
    
    Game game;
    game.run();
}

