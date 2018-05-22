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
        ConcretePoint(const Point &other): x(other.getX()), y(other.getY()) {}
        virtual int getX() const override { return x; }
        virtual int getY() const override { return y; }
};

bool operator <(const Point &a, const Point &b)
{
    /* Perform lexicographical comparison */
    if (a.getX() < b.getX()) {
        return true;
    }
    
    if (a.getX() > b.getX()) {
        return false;
    }
    
    /* This means the X coordinates are equal. */
    if (a.getY() < b.getY()) {
        return true;
    }
    
    return false;
}

#endif