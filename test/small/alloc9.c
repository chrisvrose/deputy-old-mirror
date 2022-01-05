// Checks allocation where the LHS is a void *.

#include "harness.h"

struct foo {
    int a;
    int b;
    int *p;
};

#define ALIGN 31

int main() {
    void *p;              // KEEP t1: success
    void * COUNT(size) p;   // KEEP t2: success
    struct foo *q;

    int size;
    
    size = (sizeof(*q) + ALIGN) & ~ALIGN;
    size += ALIGN;

    p = malloc(size);
    memset(p, 0, size);

    q = (struct foo * SAFE) TC(((long)p + ALIGN) & ~ALIGN);

    return q->a;
}
