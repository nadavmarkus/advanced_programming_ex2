#ifndef __CONCRETE_BOARD_H_
#define __CONCRETE_BOARD_H_

#include "Board.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include <stdlib.h>


class ConcreteBoard : public Board
{
private:
    ConcretePiecePosition board[Globals::N][Globals::M];
    
public:
    ConcreteBoard(): board()
    {
        for (size_t i = 0; i < Globals::N; ++i) {
            for (size_t j = 0; j < Globals::M; ++j) {
                board[i][j].setPoint(j, i);
            }
        }
    }

    virtual int getPlayer(const Point& pos) const override
    {
        return board[pos.getY()][pos.getX()].getPlayer();
    }
    
    void addPosition(int player, PiecePosition &position)
    {
        ConcretePiecePosition newPosition(player, position);
        board[position.getPosition().getY()][position.getPosition().getX()] = newPosition;
    }
};

#endif
