#ifndef EXTRA_FUNCTIONS_H
#define EXTRA_FUNCTIONS_H

#include <iostream>

//Function to have char inputs be turned into int when user inputs a column.
inline int char_to_int(char c)
{
    if (c >= 'A' && c <= 'J')
    {
        return toupper(c) - 'A' + 1;
    }
    return -1;
}

#endif
