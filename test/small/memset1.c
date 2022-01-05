// This test checks out our special fancy memset support.
// KEEP baseline: success

#include "harness.h"

struct foo {
    int a;
    int b;
};

struct bar {
    int a;
    int *b;
};

void * (DMEMSET(1, 2, 3) my_memset)(void *dst, int value, int sz) {
    return dst;
}

int main() {
    char * COUNT(10) pc = alloc(char, 10);
    int * COUNT(10) pi = alloc(int, 10);
    struct foo * COUNT(10) pf = alloc(struct foo, 10);
    struct bar * COUNT(10) pb = alloc(struct bar, 10);

    my_memset(pc, 0, 10);
    my_memset(pc, 42, 10);
    my_memset(pc, 0, 4);
    my_memset(pc, 0, 12); // KEEP len1: error = will always fail

    my_memset(pi, 0, 10 * sizeof(int));
    my_memset(pi, 42, 10 * sizeof(int));
    my_memset(pi, 0, 4);
    my_memset(pi, 0, 12 * sizeof(int)); // KEEP len2: error = will always fail

    my_memset(pf, 0, 10 * sizeof(struct foo));
    my_memset(pf, 42, 10 * sizeof(struct foo));
    my_memset(pf, 0, 4);
    my_memset(pi, 0, 12 * sizeof(struct foo)); // KEEP len3: error = will always fail

    my_memset(pb, 0, 10 * sizeof(struct bar));
    my_memset(pb, 42, 10 * sizeof(struct bar)); // KEEP nonzero: error = Assertion
    my_memset(pb, 0, 4); // KEEP partial: error = Assertion
    my_memset(pi, 0, 12 * sizeof(struct bar)); // KEEP len4: error = will always fail

    return 0;
}
