#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "Board.h"
#include "FightInfo.h"
#include "Move.h"
#include "ConcreteBoard.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include "PiecePosition.h"

#include <memory>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <random>
#include <assert.h>
#include <stdlib.h>

class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
    ConcreteBoard my_board_view;
    int my_player_number;
    int other_player;
    std::vector<unique_ptr<PiecePosition>> *vector_to_fill;
    bool flag_up;
    bool flag_left;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> bool_generator;
    std::uniform_int_distribution<> x_generator;
    std::uniform_int_distribution<> y_generator;
    
    void fillVectorAndUpdateBoard(int x, int y, char type, char joker_type='#')
    {
        assert(nullptr != vector_to_fill);
        ConcretePiecePosition position(my_player_number, x, y, type, joker_type);
        vector_to_fill->push_back(std::make_unique<ConcretePiecePosition>(position));
        my_board_view.addPosition(position);
    }
    
    void placeOnePieceAtRandomPosition(char type)
    {
        for (;;) {
            int x = x_generator(gen);
            int y = y_generator(gen);
            
            assert(1 <= x && x <= static_cast<int>(Globals::M));
            assert(1 <= y && y <= static_cast<int>(Globals::N));
            
            /* We count on the fact that eventually we will hit an empty spot. */
            if (my_player_number == my_board_view.getPlayerAt(x , y)) {
                continue;
            }
            
            /* All right, we are good to go. */
            fillVectorAndUpdateBoard(x, y, type);
            break;
        }
    }
    
    void placePiecesAtRandomPosition(char type)
    {
        for (size_t i = 0; i < Globals::ALLOWED_PIECES_COUNT[type]; ++i) {
            placeOnePieceAtRandomPosition(type);
        }
    }
    
    void placeMovablePieces()
    {
        placePiecesAtRandomPosition('P');
        placePiecesAtRandomPosition('R');
        placePiecesAtRandomPosition('S');
    }
    
public:
    AutoPlayerAlgorithm() : my_board_view(),
                            my_player_number(0),
                            other_player(0),
                            vector_to_fill(nullptr),
                            flag_up(false),
                            flag_left(false),
                            rd(),
                            gen(rd),
                            bool_generator(0, 1),
                            x_generator(1, Globals::M),
                            y_generator(1, Globals::N) {}

    /* Note: This algorithm assumes that there is a single flag. */
    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>> &vectorToFill) override
    {
        vector_to_fill = &vectorToFill;
        my_player_number = player;
        other_player = (2 == my_player_number) ? 1: 2;
        
        /*
         * We will position our flag in one of the corners, than start surrounding it
         * with guarding bombs and pieces.
         */
        flag_up = static_cast<bool>(bool_generator(gen));
        flag_left = static_cast<bool>(bool_generator(gen));
        
        int x, y;
        
        x = flag_left ? 1 : Globals::M;
        y = flag_up ? 1 : Globals::N;
        
        fillVectorAndUpdateBoard(x, y, 'F');
        
        /* Surround it with our two available bombs. */
        int same_row_bomb_x = (flag_left) ? 2 : Globals::M - 1;
        fillVectorAndUpdateBoard(same_row_bomb_x, y, 'B');
        
        int same_column_bomb_y = (flag_up) ? 2 : Globals::N - 1;
        fillVectorAndUpdateBoard(x, same_column_bomb_y, 'B');
        
        /* We will keep our jokers nearby as a second line of bombs. */
        int same_row_joker_x = (flag_left) ? 3 : Globals::M - 2;
        fillVectorAndUpdateBoard(same_row_joker_x, y, 'J', 'B');
        
        int same_column_joker_y = (flag_up) ? 3 : Globals::N - 2;
        fillVectorAndUpdateBoard(x, same_column_joker_y, 'J', 'B');
        
        placeMovablePieces();
        
        /* We don't copy the vector, so get rid of the pointer. */
        vector_to_fill = nullptr;
    }

};

#endif