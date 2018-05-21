#ifndef __AUTO_PLAYER_ALGORITHM_H_
#define __AUTO_PLAYER_ALGORITHM_H_

#include "PlayerAlgorithm.h"
#include "Board.h"
#include "FightInfo.h"
#include "Move.h"
#include "ConcreteBoard.h"

#include <memory>
#include <vector>
#include <map>

class AutoPlayerAlgorithm : public PlayerAlgorithm
{
private:
    ConcreteBoard my_board_view;
    int my_player_number;
    
public:
    virtual void getInitialPositions(int player, std::vector<unique_ptr<PiecePosition>>& vectorToFill) override
    {
        my_player_number = player;
        /*
         * We will position our flag in one of the corners, than start surrounding it
         * with guarding bombs and pieces. */
        
        
    }

};

#endif