#ifndef __CONCRETE_PIECE_POSITION_H_
#define __CONCRETE_PIECE_POSITION_H_

#include "PiecePosition.h"
#include "ConcretePoint.h"
#include "Point.h"

#include <memory>

class ConcretePiecePosition : public PiecePosition
{
private:
    std::unique_ptr<ConcretePoint> point;
    char type, joker_type;
    
public:
    ConcretePiecePosition(int x,
                          int y,
                          char type,
                          char joker_type) : point(std::make_unique<ConcretePoint>(x, y)),
                                             type(type),
                                             joker_type(joker_type) {}
    virtual const Point& getPosition() const override { return *point; }
    virtual char getPiece() const override { return type; }
    virtual char getJokerRep() const override { return joker_type; }
};


#endif