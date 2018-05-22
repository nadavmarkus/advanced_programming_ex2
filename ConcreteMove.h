#ifndef __CONCRETE_MOVE_H_
#define __CONCRETE_MOVE_H_

#include "Move.h"
#include "Point.h"
#include "ConcretePoint.h"

#include <memory>

class ConcreteMove : public Move
{
private:
    ConcretePoint from;
    ConcretePoint to;
public:
    ConcreteMove(int x_from,
                 int y_from,
                 int x_to,
                 int y_to): from(x_from, y_from), to(x_to, y_to) {}
    ConcreteMove(const Point &from,
                 int x_to,
                 int y_to): ConcreteMove(from.getX(), from.getY(), x_to, y_to) {}
    virtual const Point& getFrom() const override { return from; }
    virtual const Point& getTo() const override { return to; }
};

#endif