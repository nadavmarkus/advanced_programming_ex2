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
        return board[pos.getY() - 1][pos.getX() - 1].getPlayer();
    }
    
    void addPosition(const ConcretePiecePosition &position)
    {
        board[position.getPosition().getY() - 1][position.getPosition().getX() - 1] = position;
    }
    
    const ConcretePiecePosition& getPiece(const Point& point) const
    {
        return board[point.getY() - 1][point.getX() - 1];
    }
};

#endif
