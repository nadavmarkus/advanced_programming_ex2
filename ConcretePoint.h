#ifndef __CONCRETE_POINT_H_
#define __CONCRETE_POINT_H_

#include "Point.h"

class ConcretePoint : public Point
{
    private:
        int x, y;
        
    public:
        ConcretePoint() : x(0), y(0) {}
        ConcretePoint(int x, int y) : x(x), y(y) {}
        virtual int getX() const override { return x; }
        virtual int getY() const override { return y; }
};

#endif