// Makes sure that NT doesn't flow through free.

#include <stdlib.h>

int main() {
    int *p = malloc(12);
    char * NT q = malloc(12);

    free(p);
    free(q);

    p[2] = 42;

    return 0;
}
