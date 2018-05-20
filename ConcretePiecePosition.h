#ifndef __CONCRETE_PIECE_POSITION_H_
#define __CONCRETE_PIECE_POSITION_H_

#include "PiecePosition.h"
#include "ConcretePoint.h"
#include "Point.h"

#include <memory>

class ConcretePiecePosition : public PiecePosition
{
private:
    int player;
    std::unique_ptr<ConcretePoint> point;
    char type, joker_type;
    
public:
    /* Copy assignment. Used inside ConcreteBoard when adding new pieces. */
    ConcretePiecePosition& operator=(const ConcretePiecePosition &other)
    {
        if (&other == this) {
            return *this;
        }
        
        player = other.getPlayer();
        point = std::make_unique<ConcretePoint>(other.point->getX(), other.point->getY());
        type = other.getPiece();
        joker_type = other.getJokerRep();
        
        return *this;
    }
    
    ConcretePiecePosition(int player,
                          int x,
                          int y,
                          char type,
                          char joker_type) : player(player),
                                             point(std::make_unique<ConcretePoint>(x, y)),
                                             type(type),
                                             joker_type(joker_type) {}
                                             
    ConcretePiecePosition(int player, const PiecePosition &other) :ConcretePiecePosition(player,
                                                                                         other.getPosition().getX(),
                                                                                         other.getPosition().getY(),
                                                                                         other.getPiece(),
                                                                                         other.getJokerRep()) {}

    ConcretePiecePosition() : player(0), point(nullptr), type('#'), joker_type('#') {}

    const ConcretePiecePosition& operator=(ConcretePiecePosition &&other)
    {
        if (this == &other) {
            return *this;
        }
        
        type = other.getPiece();
        joker_type = other.getJokerRep();
        player = other.getPlayer();
        point = std::make_unique<ConcretePoint>(other.getPosition().getX(), other.getPosition().getY());
        
        /* Invalid the other instance now. */
        other.reset();
        
        return *this;
    }
    
    void reset(bool destroy_point = true)
    {
        type = '#';
        joker_type = '#';
        player = 0;
        if (destroy_point) {
            point = nullptr;
        }
    }
    
    virtual const Point& getPosition() const override { return *point; }
    virtual char getPiece() const override { return type; }
    virtual char getJokerRep() const override { return joker_type; }
    int getPlayer() const { return player; }
    void setPoint(int x, int y) { point = std::make_unique<ConcretePoint>(x, y); }
};


#endif