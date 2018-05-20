#ifndef __BASE_ERROR_H
#define __BASE_ERROR_H

#include <string>

class BaseError
{
private:
    std::string message;
public:
    BaseError(const std::string &message): message(message) {}
};

#endif