#ifndef __CONCRETE_BOARD_H_
#define __CONCRETE_BOARD_H_

#include "Board.h"
#include "Globals.h"
#include <stdlib.h>


class ConcreteBoard : public Board
{
    private:
    
    public:
    virtual int getPlayer(const Point& pos) const override
    {
        //TODO: Implement me.
        return 0;
    }
    
    virtual ~ConcreteBoard()
    {
    }
};

#endif
