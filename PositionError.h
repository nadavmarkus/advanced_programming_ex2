#ifndef __POSITION_ERROR_
#define __POSITION_ERROR_

#include "BaseError.h"

#include <string>
#include <stdlib.h>

class PositionError : public BaseError
{
private:
    const std::string message;
    
public:
    PositionError(const std::string &message) : BaseError(message) {}
};

#endif
