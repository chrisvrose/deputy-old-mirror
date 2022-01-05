// KEEP baseline: success

#include <stdlib.h>

int main() {
    int *p;
    int **pp;

    p = (int *) malloc(3 * sizeof(int));
    p[2] = 42;

    pp = (int **) realloc(p, 5 * sizeof(int*)); // KEEP e1: error = changes type
    p = (int *) realloc(p, 5 * sizeof(int));
    p[4] = 42;

    int res = p[4] - p[2];

    free(p);

    return res;
}
