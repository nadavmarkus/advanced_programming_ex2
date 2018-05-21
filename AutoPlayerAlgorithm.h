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

class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
    ConcreteBoard my_board_view;
    int my_player_number;
    std::vector<unique_ptr<PiecePosition>> *vector_to_fill;
    
    void fillVectorAndUpdateBoard(int x, int y, char type, char joker_type='#')
    {
        
    }
    
public:
    /* Note: This algorithm assumes that there is a single flag. */
    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>> &vectorToFill) override
    {
        std::srand(std::time(nullptr));
        my_player_number = player;
        /*
         * We will position our flag in one of the corners, than start surrounding it
         * with guarding bombs and pieces. */
        int up = std::rand() % 2;
        int left = std::rand() % 2;
        
        int x, y;
        
        x = left ? 1 : Globals::M;
        y = up ? 1 : Globals::N;
        
        ConcretePiecePosition position(my_player_number, x, y, 'F');
        vectorToFill.push_back(std::make_unique<ConcretePiecePosition>(position));
        
    }

};

#endif