#ifndef __FILE_PARSING_ERROR_
#define __FILE_PARSING_ERROR_

#include <string>
#include <stdlib.h>

class FileParsingError
{
private:
    const std::string message;
    size_t line;
    
public:
    FileParsingError(const std::string &message, size_t line) : message(message), line(line) {}
};

#endif
