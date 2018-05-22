#include "Game.h"

#include <iostream>

int main(int argc, char **argv)
{
    if (2 != argc) {
        std::cout << "Please supply the game mode: auto-vs-file, auto-vs-auto, file-vs-auto or file-vs-file" << std::endl;
        return -1;
    }
    
    std::string mode(argv[1]);
    
    bool player1_file, player2_file;
    
    if (0 == mode.compare("auto-vs-file")) {
        player1_file = false;
        player2_file = true;
        
    } else if (0 == mode.compare("auto-vs-auto")) {
        player1_file = false;
        player2_file = false;
    
    } else if (0 == mode.compare("file-vs-auto")) {
        player1_file = true;
        player2_file = false;
        
    } else if (0 == mode.compare("file-vs-file")) {
        player1_file = true;
        player2_file = true;
    
    } else {
        std::cout << "Invalid mode supplied, please supply auto-vs-file, auto-vs-auto, file-vs-auto or file-vs-file" << std::endl;
    }
    
    Game game;
    game.run(player1_file, player2_file);
}

