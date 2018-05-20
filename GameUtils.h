#ifndef __GAME_UTILS_H_
#define __GAME_UTILS_H_

namespace GameUtils
{
    bool isValidType(char type)
    {
        switch (type) {
            case 'R':
                /* Fallthrough */
            case 'P':
                /* Fallthrough */
            case 'S':
                /* Fallthrough */
            case 'B':
                /* Fallthrough */
            case 'F':
                /* Fallthrough */
            case 'J':
                return true;
                
            default:
                return false;
        }
    }
    
    bool isValidJokerMasqueradeType(char type)
    {
        switch (type) {
            case 'R':
                /* Fallthrough */
            case 'P':
                /* Fallthrough */
            case 'S':
                /* Fallthrough */
            case 'B':
                return true;
                
            default:
                return false;
        }
    }
}

#endif