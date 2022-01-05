// An optimizer test case minimized from libc/string6.

#include "harness.h"

int table[256];
int * COUNT(256) tablep = table;

const int * COUNT(256) * ctype_b_loc(void) {
    return &tablep;
}

int skip_atoi(const char * NTS * s) {
    int i = 0;

    while (((*ctype_b_loc ())[**s])) {
        i++;
    }

    return i;
}

const char * NTS make_str() {
    char * NTS str = malloc(1);
    str[0] = 0;
    return str;
}

int main() {
    const char * NTS p;
    int i;

    for (i = 0; i < 256; i++) {
        table[i] = 0;
    }
    
    p = make_str();
    skip_atoi(&p);

    return 0;
}
