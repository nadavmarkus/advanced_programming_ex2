#ifndef __CONCRETE_BOARD_H_
#define __CONCRETE_BOARD_H_

#include "Board.h"
#include "Globals.h"
#include "ConcretePiecePosition.h"
#include <stdlib.h>
#include <utility>
#include <iostream>


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
    
    const ConcretePiecePosition& getPiece(const Point &point) const
    {
        return board[point.getY() - 1][point.getX() - 1];
    }
    
    void movePiece(const Point &from, const Point &to)
    {
        board[to.getY() - 1][to.getX() - 1] = std::move(board[from.getY() - 1][from.getX() - 1]);
    }
    
    void invalidatePosition(const Point &where)
    {
        board[where.getY() - 1][where.getX() - 1].reset(false);
    }
    
    void updateJokerPiece(const Point &where, char new_joker_type)
    {
        board[where.getY() - 1][where.getX() - 1].setJokerRep(new_joker_type);
    }
    
    void printBoard() const
    {
        for (size_t i = 0; i < Globals::N; ++i) {
            for (size_t j = 0; j < Globals::M; ++j) {
                std::cout << board[i][j].getPiece() << " ";
            }
            std::cout << std::endl;
        }
    }
};

#endif
