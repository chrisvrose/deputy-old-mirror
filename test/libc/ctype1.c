
#include <stdio.h>
#include <stdlib.h>

//  __USE_ISOC99 gives us 'isblank'
#define __USE_ISOC99
#include <ctype.h>

#include "harness.h"

int main() {
    char c = 'a';
    if(! isalpha(c)) E(1);
    if(isalpha('$')) E(2)

    if(isblank(c)) E(3);
    if(! isblank(' ')) E(4);

    if(toupper(c) != 'A') E(5);
    if(tolower('A') != c) E(6);

    if(isdigit(c))  E(7);
    if(! isdigit('5')) E(8);

    return 0;
}
