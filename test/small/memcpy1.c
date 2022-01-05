// This test checks out our special fancy memcpy support.
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

void * (DMEMCPY(1, 2, 3) my_memcpy)(void *dst, void *src, int sz) {
    return dst;
}

int main() {
    char * COUNT(10) pc1 = alloc(char, 10);
    char * COUNT(10) pc2 = alloc(char, 10);

    int * COUNT(10) pi1 = alloc(int, 10);
    int * COUNT(10) pi2 = alloc(int, 10);

    struct foo * COUNT(10) pf1 = alloc(struct foo, 10);
    struct foo * COUNT(10) pf2 = alloc(struct foo, 10);

    struct bar * COUNT(10) pb1 = alloc(struct bar, 10);
    struct bar * COUNT(10) pb2 = alloc(struct bar, 10);


    my_memcpy(pc1, pc2, 10);
    my_memcpy(pc1, pc2, 4);
    my_memcpy(pc1, pc2, 12); // KEEP len1: error = will always fail

    my_memcpy(pi1, pi2, 10 * sizeof(int));
    my_memcpy(pi1, pi2, 4 * sizeof(int));
    my_memcpy(pi1, pi2, 12 * sizeof(int)); // KEEP len2: error = will always fail

    my_memcpy(pi1, pc1, 10);
    my_memcpy(pc1, pi1, 10);

    my_memcpy(pi1, pc1, 10 * sizeof(int)); // KEEP len3: error = will always fail
    my_memcpy(pc1, pi1, 10 * sizeof(int)); // KEEP len4: error = will always fail

    my_memcpy(pf1, pf2, 10 * sizeof(struct foo));
    my_memcpy(pf1, pf2, 4);
    my_memcpy(pf1, pf2, 12 * sizeof(struct foo)); // KEEP len5: error = will always fail

    my_memcpy(pb1, pb2, 10 * sizeof(struct bar));
    my_memcpy(pb1, pb2, 4); // KEEP partial1: error = Assertion
    my_memcpy(pb1, pb2, 12 * sizeof(struct bar)); // KEEP len5: error = will always fail

    my_memcpy(pf1, pb1, 10 * sizeof(struct bar));
    my_memcpy(pb1, pf1, 10 * sizeof(struct bar)); // KEEP type1: error = different base types

    return 0;
}
